# Viabilidad: ¿puede un arnés hacer más seguro el desarrollo en C?

**Conclusión.** Sí se puede hacer más seguro, y la parte medida es prometedora. No se puede hacer seguro en sentido estricto: C permite por diseño construcciones que ningún arnés de este tipo cierra sin convertirlo en otro lenguaje. Por eso el arnés declara sus límites en lugar de esconderlos. La conclusión es provisional: se apoya en 79 celdas pequeñas, el propio verificador y auditorías incompletas de [cJSON](AUDITORIA-CJSON.md), [lz4](AUDITORIA-LZ4.md) y [zlib](AUDITORIA-ZLIB.md).

## Qué cambió la pregunta

La primera versión de este documento preguntaba si C podía volverse seguro y proponía cerrar los huecos prohibiendo punteros crudos, aritmética de punteros y `void *`. La premisa [FUN-1](premisas_c-harness.md) descartó ese camino, porque esas construcciones son C. Ahora se distinguen dos cosas:

- **Fallo:** el arnés debería detenerlo y aún no lo hace. Es trabajo pendiente.
- **Límite:** C lo permite por diseño y el arnés no lo garantiza. Se declara en una celda de tipo `limit`.

Pasar una celda a límite es una decisión visible en el historial, no una forma de mejorar el resultado. Las mediciones anteriores se conservan en [results/](../results/). Las anteriores a `measurement-2026-11-02d.md` usan los nombres antiguos de las celdas, en español, porque son registros históricos.

## Qué se midió

| Resultado | Sin arnés | Con arnés |
|---|---|---|
| Programas incorrectos detenidos | 18 de 59 | **55 de 59** |
| Controles válidos aceptados | 20 de 20 | **20 de 20** |
| Límites declarados | | 4 |
| Celdas con aviso | | 3, de las cuales 1 es un control válido |

Los 18 de la columna «sin arnés» incluyen programas que terminaron con un código distinto de cero por azar. Cada rechazo debe coincidir con un motivo esperado escrito en la celda; si se detiene por otra razón, cuenta como fallo. La tabla completa está en [measurement-2026-11-02g.md](../results/measurement-2026-11-02g.md).

**Estas cifras son una prueba de regresión, no una tasa de detección.** Las celdas se escribieron junto con los mecanismos que las detienen. Las medidas más reveladoras son las auditorías: ni [cJSON](AUDITORIA-CJSON.md), ni [lz4](AUDITORIA-LZ4.md), ni [zlib](AUDITORIA-ZLIB.md) mostraron un error de memoria confirmado, y cJSON y zlib destaparon siete defectos del arnés que las celdas no habían detectado. Esos programas son evidencia y no modelos: lo observado, bueno o malo, se recoge en [PRACTICAS-OBSERVADAS.md](PRACTICAS-OBSERVADAS.md).

## Qué consigue el arnés

- **El verificador se pasa a sí mismo.** [ch_verify.c](../verifier/ch_verify.c) está escrito en C y supera las cinco etapas. Si se le quita un `const`, [self-check.sh](../self-check.sh) lo detiene.
- **Cuatro reglas propias.** CONST-1, ALIAS-1, STATE-1 y NAME-1 cubren lo que GCC no comprueba en estas celdas.
- **C decide, no un modelo propio.** Para la regla CONST-1, el verificador inserta `const` en el parámetro y vuelve a analizar el archivo. Si sigue compilando, el `const` era posible. Así respeta escrituras, reenvíos, `free` y firmas fijadas por punteros a función sin programar esos casos.
- **La superficie de efectos se puede auditar.** `nm` y una lista cerrada de símbolos detienen funciones de libc declaradas a mano, y también los hilos.
- **La búsqueda acotada alcanza caminos no ejecutados.** CBMC detuvo cruces de límites por aritmética de punteros y dentro de una estructura, y una escritura en una dirección inventada que solo ocurre con `argc > 5`.
- **Prohibir por nombre resiste bien.** `#pragma GCC poison` resistió concatenación de tokens, alias y atributos, y cada prohibición declara su razón.
- **Muchos errores comunes caen.** Desbordamientos de enteros, división entre cero, doble liberación, fugas y uso tras `free` se detuvieron en las celdas.

## El aviso por `argc`

CBMC considera cualquier valor de `argc`, incluido el máximo, y por eso informa de que `argc + 1` puede desbordar, algo que no ocurre en un sistema real. Ahora, si un fallo desaparece al limitar `argc` a 1 048 576, el programa pasa con un **aviso** en la tabla. Esta cota es una decisión, no una garantía para todo sistema. La celda `ub_argc_overflow_within_bound` comprueba que un desbordamiento posible dentro de la cota se sigue rechazando.

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
- **CBMC** comprueba todos los caminos, pero hasta una profundidad acotada y sobre su propio modelo. La etapa usa 5 vueltas y `--no-unwinding-assertions`, así que un éxito no es una demostración, y puede rechazar código válido. Sus resultados dependen de ese modelo: el verificador tuvo que recibir un `argv` válido en [cbmc_entry.c](../verifier/cbmc_entry.c), y CBMC no entra en libclang. **En el verificador, CBMC solo alcanza el 15,7 % del código**, porque nunca ejecuta los recorridos del árbol. En cJSON y zlib no concluyó. En lz4 concluyó, pero sus lecturas fuera de límites revisadas resultaron ser efecto de cómo modela un puntero anterior al objeto.
- **El verificador** tiene cuatro reglas de alcance limitado. CONST-1 es superficial como el propio `const` de C: no ve una escritura a través de un puntero guardado en una estructura. ALIAS-1 solo mira `&x` directos en llamadas a funciones con nombre.

## Límites de esta medición

- Las celdas y el arnés tienen el mismo origen. Falta una revisión adversaria independiente.
- Solo se ha auditado un programa ajeno, y parcialmente. El coste real de adoptar el arnés sigue sin medirse.
- La regla CONST-1 omite por ahora los dobles punteros y los parámetros declarados como arreglo. Tampoco reescribe declaraciones en cabeceras del sistema o generadas por macros; en esos casos avisa de que no analizó el parámetro.
- Una cabecera que no sea C estándar, como las de POSIX, suele rechazarse como efecto secundario del veneno, no por una decisión explícita.
- El verificador confía en GCC, libclang y libc, y Bash sigue coordinando las etapas.
- CBMC no analiza módulos sin `main`.
- Frama-C, AFL++, Valgrind, Cppcheck y clang-tidy están instalados, pero aún no se han medido contra las celdas.

## Siguientes pasos

1. **Correr la etapa de ejecución de cJSON, lz4 y zlib**, cuando el autor la autorice.
2. **Ampliar la muestra de programas pulidos**, como zlib, y y cerrar las observaciones del registro de [las premisas](premisas_c-harness.md) solo con evidencia.
3. **Mostrar las relaciones de punteros de CBMC como causa probable** de los fallos posteriores.
4. **Introducir errores conocidos** en código real y medir cuántos detecta el arnés, para obtener una tasa de detección que signifique algo.
5. **Distinguir los fallos contenidos** de forma automática, según FUN-3.
6. **Medir el coste en rendimiento**, según FUN-6, empezando por lz4 con y sin los cambios que exige el arnés.
7. **Prototipar la biblioteca de arenas** contra `mem8_after_quarantine`.
8. **Medir Frama-C, AFL++ y las demás herramientas.**
