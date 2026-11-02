# Viabilidad: ¿puede un arnés hacer más seguro el desarrollo en C?

**Conclusión.** Sí se puede hacer más seguro, y la parte medida es prometedora. No se puede hacer seguro en sentido estricto: C permite por diseño construcciones que ningún arnés de este tipo cierra sin convertirlo en otro lenguaje. Por eso el arnés declara sus límites en lugar de esconderlos. La conclusión es provisional: se apoya en 70 celdas pequeñas y un solo programa real, el propio verificador.

## Qué cambió la pregunta

La primera versión de este documento preguntaba si C podía volverse seguro y proponía cerrar los huecos prohibiendo punteros crudos, aritmética de punteros y `void *`. La premisa [BASE-1](PREMISAS.md) descartó ese camino, porque esas construcciones son C. Ahora se distinguen dos cosas:

- **Fallo:** el arnés debería detenerlo y aún no lo hace. Es trabajo pendiente.
- **Límite:** C lo permite por diseño y el arnés no lo garantiza. Se declara en una celda de tipo `limit`.

Pasar una celda a límite es una decisión visible en el historial, no una forma de mejorar el resultado. Las mediciones anteriores se conservan en [results/](../results/). Las fechadas antes de `measurement-2026-11-02d.md` usan los nombres antiguos de las celdas, en español, porque son registros históricos.

## Qué se midió

| Resultado | Sin arnés | Con arnés |
|---|---|---|
| Programas incorrectos detenidos | 17 de 56 | **52 de 56** |
| Controles válidos aceptados | 14 de 14 | **13 de 14** |
| Límites declarados | | 4 |

Los 17 de la columna «sin arnés» incluyen programas que terminaron con un código distinto de cero por azar. Cada rechazo debe coincidir con un motivo esperado escrito en la celda; si se detiene por otra razón, cuenta como fallo. Se comprobó alterando el motivo de una celda. La tabla completa está en [measurement-2026-11-02d.md](../results/measurement-2026-11-02d.md).

Estas cifras no se comparan directamente con las anteriores, porque el conjunto de celdas cambió. Las celdas nuevas de las reglas y los límites se escribieron junto con los mecanismos que las detienen, así que confirman el diseño, pero no estiman cuánto detectaría el arnés en código desconocido.

## Qué consigue el arnés

- **El verificador se pasa a sí mismo.** [ch_verify.c](../verifier/ch_verify.c) está escrito en C y supera las cinco etapas. Si se le quita un `const`, [self-check.sh](../self-check.sh) lo detiene.
- **Cuatro reglas propias.** CONST-1, ALIAS-1, STATE-1 y NAME-1 cubren lo que GCC no comprueba en estas celdas.
- **C decide, no un modelo propio.** Para la regla CONST-1, el verificador inserta `const` en el parámetro y vuelve a analizar el archivo. Si sigue compilando, el `const` era posible. Así respeta escrituras, reenvíos, `free` y firmas fijadas por punteros a función sin programar esos casos.
- **La superficie de efectos se puede auditar.** `nm` y una lista cerrada de símbolos detienen funciones de libc declaradas a mano, y también los hilos.
- **La búsqueda acotada alcanza caminos no ejecutados.** CBMC detuvo cruces de límites por aritmética de punteros y dentro de una estructura, y una escritura en una dirección inventada que solo ocurre con `argc > 5`.
- **Prohibir por nombre resiste bien.** `#pragma GCC poison` resistió concatenación de tokens, alias y atributos, y cada prohibición declara su razón.
- **Muchos errores comunes caen.** Desbordamientos de enteros, división entre cero, doble liberación, fugas y uso tras `free` se detuvieron en las celdas.

## Fallo pendiente: un falso positivo

`ub_control_argc_plus_one` es un programa válido y el arnés lo rechaza. CBMC considera cualquier valor de `argc`, incluido el máximo, y por eso informa de que `argc + 1` puede desbordar, algo que no ocurre en un sistema real.

Es una decisión de diseño abierta. Desactivar esa comprobación en CBMC elimina el falso positivo, pero los desbordamientos solo se detectarían en los caminos que se ejecuten. La otra opción es modelar `argc` en un punto de entrada con un máximo razonable. Mientras se decide, la celda sigue en rojo.

## Límites declarados

| Celda | Por qué C lo permite | Qué lo reduce |
|---|---|---|
| `mem4_indirect_pointer` | Una dirección puede pasar por variables intermedias | STATE-1 solo ve la dirección directa; seguir su rastro exigiría análisis de flujo |
| `mem5_output_via_pointer_variable` | Pasar `q` no muestra `&` en la llamada | Convención de nombres, sin medir |
| `typ_void_confusion` | `void *` borra el tipo | CBMC no lo detectó: el acceso cae dentro del objeto |
| `mem8_after_quarantine` | `free` no invalida las copias del puntero | Biblioteca opcional de arenas generacionales, sin construir |

La validez temporal es el límite más serio: un puntero colgante que escribe en un bloque reutilizado no lo vio ninguna herramienta. El «éxito» de CBMC en esa celda **no cuenta**: se limitaron los bucles a 5 vueltas y su modelo no reutiliza direcciones, así que nunca alcanza el error.

## Por qué ninguna herramienta basta sola

- **Los sanitizadores** solo ven lo que se ejecuta y sus defensas son probabilísticas.
- **El analizador de GCC** no es exhaustivo y da falsos positivos: informó fugas en celdas con punteros `volatile`.
- **CBMC** comprueba todos los caminos, pero hasta una profundidad acotada y sobre su propio modelo. La etapa usa 5 vueltas y `--no-unwinding-assertions`, así que un éxito no es una demostración, y puede rechazar código válido. Sus resultados dependen de ese modelo: el verificador tuvo que recibir un `argv` válido en [cbmc_entry.c](../verifier/cbmc_entry.c), y CBMC no entra en libclang.
- **El verificador** tiene cuatro reglas de alcance limitado. CONST-1 es superficial como el propio `const` de C: no ve una escritura a través de un puntero guardado en una estructura. ALIAS-1 solo mira `&x` directos en llamadas a funciones con nombre.

## Límites de esta medición

- Las celdas y el arnés tienen el mismo origen. Falta una revisión adversaria independiente.
- Solo hay un programa real. El coste de usar el arnés en código ajeno sigue sin medirse.
- La regla CONST-1 omite por ahora los dobles punteros, los parámetros declarados como arreglo y las declaraciones en cabeceras o generadas por macros. En estos dos últimos casos avisa de que no analizó el parámetro.
- Una cabecera fuera de la lista permitida suele rechazarse como efecto secundario del veneno, no por una decisión explícita.
- El verificador confía en GCC, libclang y libc, y Bash sigue coordinando las etapas.
- CBMC no analiza módulos sin `main`.
- Frama-C, AFL++, Valgrind, Cppcheck y clang-tidy están instalados, pero aún no se han medido contra las celdas.

## Siguientes pasos

1. **Decidir cómo tratar el falso positivo de `argc`.**
2. **Portar un programa real ajeno** de unos cientos de líneas. Es la prueba decisiva y ahora la más urgente: los controles no bastan para saber cuánto código legítimo rechaza el arnés.
3. **Extender CONST-1** a cabeceras y declaraciones de arreglo.
4. **Prototipar la biblioteca de arenas** contra `mem8_after_quarantine`.
5. **Medir Frama-C, AFL++ y las demás herramientas** contra las celdas.
