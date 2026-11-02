# Viabilidad: ¿puede un arnés hacer más seguro el desarrollo en C?

**Conclusión.** Sí se puede hacer más seguro, y la parte medida es prometedora. No se puede hacer seguro en sentido estricto: C permite por diseño construcciones que ningún arnés de este tipo cierra sin convertirlo en otro lenguaje. Por eso el arnés declara sus límites en lugar de esconderlos. La conclusión es provisional: se apoya en 61 celdas pequeñas y un solo programa real, el propio verificador.

## Qué cambió la pregunta

La primera versión de este documento preguntaba si C podía volverse seguro y proponía cerrar los huecos prohibiendo punteros crudos, aritmética de punteros y `void *`. La premisa [BASE-1](PREMISAS.md) descartó ese camino, porque esas construcciones son C. Ahora se distinguen dos cosas:

- **Fallo:** el arnés debería detenerlo y aún no lo hace. Es trabajo pendiente.
- **Límite:** C lo permite por diseño y el arnés no lo garantiza. Se declara en una celda de tipo `limite`.

Pasar una celda a límite es una decisión visible en el historial, no una forma de mejorar el resultado. La medición original se conserva en [medicion-2026-11-02.md](../resultados/medicion-2026-11-02.md).

## Qué se midió

| Resultado | Sin arnés | Con arnés |
|---|---|---|
| Programas incorrectos detenidos | 17 de 52 | **43 de 52** |
| Controles válidos aceptados | 9 de 9 | **9 de 9** |
| Fallos pendientes | | 3 |
| Límites declarados | | 6 |

Los 17 de la columna «sin arnés» incluyen programas que terminaron con un código distinto de cero por azar. Cada rechazo debe coincidir con un motivo esperado escrito en la celda; si se detiene por otra razón, cuenta como fallo. Se comprobó alterando el motivo de una celda. La tabla completa está en [medicion-2026-11-02b.md](../resultados/medicion-2026-11-02b.md).

## Qué consigue el arnés

- **El verificador se pasa a sí mismo.** [ch_verificar.c](../verificador/ch_verificar.c) está escrito en C y supera las cuatro etapas. Si se le quita un `const`, [autoverificar.sh](../autoverificar.sh) lo detiene.
- **C decide, no un modelo propio.** Para la regla CONST-1, el verificador inserta `const` en el parámetro y vuelve a analizar el archivo. Si sigue compilando, el `const` era posible. Así respeta escrituras, reenvíos, `free` y firmas fijadas por punteros a función sin programar esos casos.
- **La superficie de efectos se puede auditar.** `nm` y una lista cerrada de símbolos detienen funciones de libc declaradas a mano, y también los hilos.
- **Prohibir por nombre resiste bien.** `#pragma GCC poison` resistió concatenación de tokens, alias y atributos, y cada prohibición declara su razón.
- **Muchos errores comunes caen.** Desbordamientos de enteros, división entre cero, doble liberación, fugas y uso tras `free` se detuvieron en las celdas.

## Fallos pendientes

| Celda | Problema | Vía propuesta, sin comprobar |
|---|---|---|
| `mem4_puntero_al_estado` | Un módulo entrega un puntero a su estado | Regla del verificador: la dirección de un `static` no sale de la función |
| `mem5_misma_salida_dos_veces` | Una variable entra como dos salidas | Regla del verificador: comparar los argumentos `&x` de una llamada |
| `mem7_variable_como_funcion` | Una variable oculta a una función | Regla del verificador: colisión de nombres |

## Límites declarados

| Celda | Por qué C lo permite | Qué lo reduce |
|---|---|---|
| `lim_aritmetica_puntero` | La aritmética de punteros es parte de C | CBMC lo detectó; todavía no es una etapa del arnés |
| `lim_dentro_de_estructura` | Un arreglo decae a puntero y pierde su tamaño | CBMC lo detectó; todavía no es una etapa del arnés |
| `tip_confusion_void` | `void *` borra el tipo | CBMC no lo detectó: el acceso cae dentro del objeto |
| `tip_entero_a_puntero` | Entero y dirección se convierten entre sí | CBMC lo detectó en una prueba aislada; todavía no es una etapa |
| `mem5_salida_por_variable_puntero` | Pasar `q` no muestra `&` en la llamada | Convención de nombres, sin medir |
| `mem8_tras_cuarentena` | `free` no invalida las copias del puntero | Biblioteca opcional de arenas generacionales, sin construir |

La validez temporal es el límite más serio: un puntero colgante que escribe en un bloque reutilizado no lo vio ninguna herramienta. El «éxito» de CBMC en esa celda **no cuenta**: se limitaron los bucles a 5 vueltas y su modelo no reutiliza direcciones, así que nunca alcanza el error.

## Por qué ninguna herramienta basta sola

- **Los sanitizadores** solo ven lo que se ejecuta y sus defensas son probabilísticas.
- **El analizador de GCC** no es exhaustivo y da falsos positivos: informó fugas en celdas con punteros `volatile`.
- **CBMC** comprueba todos los caminos, pero hasta una profundidad acotada y sobre su propio modelo de memoria. Las mediciones usaron `--no-unwinding-assertions`, así que un éxito no es una demostración.
- **El verificador** tiene hoy una sola regla, y esta regla es superficial como el propio `const` de C: no ve una escritura a través de un puntero guardado en una estructura.

## Límites de esta medición

- Las celdas y el arnés tienen el mismo origen. Falta una revisión adversaria independiente.
- Solo hay un programa real. El coste de usar el arnés en código ajeno sigue sin medirse.
- La regla CONST-1 omite por ahora los dobles punteros, los parámetros declarados como arreglo y las declaraciones en cabeceras o generadas por macros. En estos dos últimos casos avisa de que no analizó el parámetro.
- Una cabecera fuera de la lista permitida suele rechazarse como efecto secundario del veneno, no por una decisión explícita.
- El verificador confía en GCC, libclang y libc, y Bash sigue coordinando las etapas.
- Frama-C, AFL++, Valgrind, Cppcheck y clang-tidy están instalados, pero aún no se han medido contra las celdas.

## Siguientes pasos

1. **Añadir CBMC como etapa del arnés**, con límites explícitos, y medir su coste.
2. **Cerrar los 3 fallos pendientes** con nuevas reglas del verificador.
3. **Extender CONST-1** a cabeceras y declaraciones de arreglo.
4. **Portar un programa real ajeno** de unos cientos de líneas. Es la prueba decisiva: si el arnés resulta inutilizable, la idea falla aunque todas las celdas pasen.
5. **Prototipar la biblioteca de arenas** contra `mem8_tras_cuarentena`.
