# Auditoría de lz4 v1.10.0

Esta auditoría pasa por el arnés [lz4](https://github.com/lz4/lz4), un compresor en C muy usado, centrado en la velocidad. Se analizó `lib/lz4.c`, de 2829 líneas, sin modificarlo; no se incluye en este repositorio. El resultado literal está en [audit-lz4-v1.10.0.md](../results/audit-lz4-v1.10.0.md) y, con la ejecución, en [audit-lz4-v1.10.0-run.md](../results/audit-lz4-v1.10.0-run.md).

**Conclusión.** **No se confirmó ningún desbordamiento en lz4.** CBMC informó de 29 fallos con entradas pequeñas, pero las lecturas fuera de límites revisadas vienen de cómo su modelo representa un puntero anterior al objeto. Lo que sí queda es comportamiento indefinido técnico: lz4 forma y compara punteros fuera de los objetos.

**Alcance.** Es incompleta: la ejecución con sanitizadores nunca le dio al descompresor datos corruptos. No se revisaron uno por uno todos los fallos de CBMC, solo los caminos descritos aquí.

## Qué encontró cada etapa

| Etapa | Resultado | Lectura |
|---|---|---|
| Compilación: veneno | 3 usos de `__attribute__` | lz4 no usa ninguna de las funciones sin límite vetadas |
| Compilación: avisos | 4 de `-Wanalyzer-null-dereference` en la línea 946; 1 de `-Wmissing-prototypes` | Sin revisar: el analizador tiene falsos positivos conocidos |
| Verificador | 2 de CONST-1 | Parámetros de dos funciones que parecen conservarse por compatibilidad; sin revisar a fondo |
| Símbolos | Sin hallazgos | Sin estado mutable y sin funciones externas fuera de la lista |
| CBMC | 29 fallos con 2 o 4 caracteres; 15 con 19 | Ver la sección siguiente |
| Ejecución | 16 entradas, ninguna detenida | Comprime y descomprime archivos de `lib/`, y todos los resultados coinciden con el original. **El descompresor solo recibe datos válidos**, así que esto no prueba su parte más expuesta |

## El análisis de los fallos de CBMC

Los fallos parecían graves: lecturas y escrituras fuera de límites en el descompresor de una biblioteca muy revisada. Se investigaron como una hipótesis, sin ejecutar lz4.

1. **Un contraejemplo.** Con 3 bytes de entrada y el token 45, CBMC entra en un atajo que exige `ip < shortiend`, donde `shortiend = iend - 16`. Ese puntero queda antes del inicio del arreglo. En una máquina real la comparación daría falso y no se entraría.
2. **Prueba con más entrada.** Desde 16 bytes, `shortiend` queda dentro del arreglo y desaparecen sus fallos y los de `LZ4_read16`. Los demás siguen.
3. **El valor revelador.** En la lectura de `match` de la línea 2408, la traza muestra `match = output + 72057594037927932`. Ese número es 2⁵⁶ − 4, es decir, `output - 4` representado como un desplazamiento sin signo.
4. **La consecuencia.** La línea 2356 comprueba `match + dictSize < lowPrefix` justamente para rechazar ese caso. Con el desplazamiento enorme, el modelo da la comprobación por superada, y por eso llega a la lectura.

## Qué queda en pie

- **Comportamiento indefinido técnico.** C no permite formar un puntero fuera de su arreglo, más allá de la posición siguiente al último elemento, ni compararlo. lz4 lo hace. Funciona con memoria plana, pero depende de algo que la norma no garantiza. Esta auditoría no valora si es un riesgo práctico en alguna plataforma.
- **No es un informe de vulnerabilidad.** Nada de lo anterior demuestra un acceso indebido real.
- **Es un fallo contenido, en el sentido de FUN-3.** La seguridad del código depende de una comprobación posterior y de cómo se comportan los punteros en memoria plana, no de una forma que una regla pueda comprobar.

## Lo que esto cambia en el arnés

- **CBMC sí concluye en este programa**, a diferencia de cJSON, en pocos segundos.
- **Sus fallos necesitan revisión.** Un fallo de `pointer relation` puede invalidar los fallos de lectura posteriores del mismo camino. Rechazar por ellos sin revisar llevaría a conclusiones falsas. Conviene que el arnés muestre primero esas relaciones como causa probable.
- **Para repetirla:** `./audit/audit.sh lz4`, o con `--run` para añadir la ejecución.
