# Viabilidad: ¿puede un arnés hacer seguro a C?

**Conclusión.** Un arnés sobre C completo no puede garantizar seguridad de memoria. Un arnés sobre un **subconjunto estricto** de C parece viable en lo espacial y en los tipos, pero sigue sin resolver lo temporal. Esta conclusión es provisional: se apoya en 56 celdas pequeñas y en un solo compilador.

## Qué se midió

Se escribieron 56 celdas: 5 programas válidos de control, 29 violaciones directas y 22 evasiones, escritas expresamente para burlar el arnés. Cada una se ejecutó sin arnés y con él. La tabla completa está en [la medición del 2 de noviembre de 2026](../resultados/medicion-2026-11-02.md).

| Resultado | Sin arnés | Con arnés |
|---|---|---|
| Programas incorrectos detenidos | 17 de 51 | **40 de 51** |
| Controles válidos aceptados | 5 de 5 | **5 de 5** |

Los 17 de la columna «sin arnés» no son todos detecciones: incluyen programas que terminaron con un código distinto de cero por puro azar del comportamiento indefinido.

## Lo que el arnés sí consigue

- **Prohibir por nombre funciona bien.** `#pragma GCC poison` resistió la concatenación de tokens, los alias y los atributos. También impide silenciar avisos con `#pragma GCC diagnostic` o `system_header`.
- **El cruce marcado en ambos extremos es posible.** Con un `struct` envoltorio y una macro, olvidar la marca en la llamada o en la firma es un error de tipos. MEM-5 se traslada a C estándar sin herramientas adicionales.
- **`nm` encuentra el estado global.** Inspeccionar el objeto compilado detecta variables exportadas y `static` ocultos, sin analizar el código fuente.
- **Muchos errores comunes caen.** Desbordamientos de enteros, división entre cero, doble liberación, fugas y uso tras `free` directo se detuvieron en las celdas.

## Lo que el arnés no consigue

Estas 11 celdas pasaron sin detección. La última columna es una **hipótesis todavía no comprobada**.

| Celda | Problema | Vía de cierre propuesta |
|---|---|---|
| `mem5_puntero_crudo` | `int *` evita la marca de salida | Verificador: prohibir punteros crudos en las firmas |
| `mem5_misma_salida_dos_veces` | Una variable entra como dos salidas | Verificador: comparar los argumentos marcados |
| `mem4_puntero_al_estado` | El módulo entrega un puntero a su estado | Verificador: prohibir que escape la dirección de un `static` |
| `mem7_variable_como_funcion` | Una variable oculta a una función | Verificador: colisión de nombres |
| `lim_aritmetica_puntero` | Acceso fuera de límites mediante aritmética | Verificador: prohibir la aritmética de punteros y usar segmentos con longitud |
| `lim_dentro_de_estructura` | Se pisa otro campo de la misma estructura | Verificador: prohibir que un arreglo decaiga a puntero |
| `tip_confusion_void` | `void *` cambia de tipo sin aviso | Verificador: prohibir `void *` fuera del asignador |
| `tip_entero_a_puntero` | Un entero se convierte en dirección | Verificador: prohibir esa conversión |
| `agt4_efecto_no_listado` | Una declaración a mano llega a libc | `nm`: lista cerrada de símbolos externos permitidos |
| `ub_carrera_de_datos` | Dos hilos escriben sin sincronizar | Lista de símbolos: excluir hilos por ahora |
| `mem8_tras_cuarentena` | Un puntero colgante escribe en un bloque reutilizado | **Sin vía dentro de este arnés** |

## Por qué los sanitizadores no bastan

- **Solo ven lo que se ejecuta.** Un error en un camino que las pruebas no recorren no se detecta. Son pruebas, no garantías.
- **Sus defensas son probabilísticas.** `mem8_tras_cuarentena` asigna y libera memoria hasta que AddressSanitizer reutiliza el bloque. A partir de ahí, escribir con el puntero viejo es invisible.
- **No sirven como protección en producción sin evaluarlo aparte.** Su coste y su superficie propia quedan sin medir aquí.

## Por qué el analizador tampoco basta

`-fanalyzer` encontró errores reales, pero no es exhaustivo y se equivoca en ambos sentidos. Durante este experimento informó fugas en celdas con punteros `volatile`, lo que ocultó durante una ronda el verdadero resultado de esas celdas. Por eso la tabla muestra ahora el **motivo** de cada rechazo: un rechazo por la razón equivocada no prueba nada.

## El problema de fondo: la memoria temporal

Límites, tipos y efectos se pueden restringir con reglas locales. La validez temporal, no: saber si un puntero sigue vivo depende de lo que ocurrió antes, potencialmente en otra parte del programa. Hay tres caminos, y cada uno tiene un coste:

| Camino | Idea | Coste |
|---|---|---|
| Arenas con índices generacionales | No hay `free` individual ni punteros persistentes; cada acceso comprueba la generación | Cambia la forma de programar; añade una comprobación por acceso. **Es la opción recomendada para probar primero** |
| Propiedad y préstamos | Un verificador estático al estilo de Rust | Grande: es casi otro lenguaje |
| Compilador o hardware con punteros comprobados (Fil-C, CHERI) | La protección se aplica por debajo del lenguaje | Dependencia externa y coste de rendimiento; no evaluado aquí |

## Límites de esta medición

- Las celdas las escribió la misma parte que construyó el arnés. Falta una revisión adversaria independiente.
- Son programas de pocas líneas. El analizador y los falsos positivos pueden comportarse peor en código real.
- Cinco controles no bastan para estimar cuánto código legítimo rechaza el arnés.
- La tabla muestra el motivo de cada rechazo, pero `correr.sh` todavía no comprueba que sea el esperado. Esa revisión fue manual.
- Falta probar Clang, Frama-C y CBMC, que no estaban instalados. Lo dicho sobre Fil-C y CHERI procede de conocimiento general, no de una medición.

## Siguientes pasos

1. **Aceptar o corregir las premisas** de [PREMISAS.md](PREMISAS.md).
2. **Hacer que cada celda declare su motivo esperado**, para que un rechazo por otra razón cuente como fallo.
3. **Cerrar con `nm` la lista de símbolos externos.** Es lo más barato y cubre dos celdas.
4. **Construir el verificador sintáctico** y comprobar si cierra las otras ocho celdas.
5. **Prototipar arenas generacionales** contra `mem8_tras_cuarentena`.
6. **Portar un programa real** de unos cientos de líneas. Es la prueba decisiva: si el subconjunto resulta inutilizable, la idea falla aunque todas las celdas pasen.
