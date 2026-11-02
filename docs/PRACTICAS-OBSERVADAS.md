# Prácticas observadas en C

Este documento recoge lo observado en programas reales, tanto lo bueno como lo malo. Los programas estudiados son **evidencia, no modelos**: según [FUN-3](premisas_c-harness.md), un programa puede ser seguro solo porque años de parches, pruebas exhaustivas y mucha experiencia contienen construcciones peligrosas. El objetivo de C-Harness es no depender de esa contención.

**Conclusión provisional.** Estudiar programas pulidos sirve como fuente de hipótesis y ya ha corregido decisiones del arnés, pero «pulido» no equivale a «buena práctica». Los dos programas coinciden en algunas prácticas, discrepan en otras y contienen formas que la norma de C no permite. Los veredictos se mantienen en el [registro de las premisas](premisas_c-harness.md#6-registro-de-lo-observado). Con dos programas, todo lo que sigue es una **hipótesis**.

## Qué se comparó

Se analizaron dos proyectos estables y sin modificar, de autores y propósitos distintos: [cJSON v1.7.19](AUDITORIA-CJSON.md), un analizador de JSON centrado en la API, y [lz4 v1.10.0](AUDITORIA-LZ4.md), un compresor centrado en la velocidad. Los recuentos los genera [practices.py](../audit/practices.py), un medidor exploratorio con heurísticas simples que no forma parte del arnés.

| Medida | cJSON | lz4 |
|---|---|---|
| Funciones | 113 | 90 |
| Funciones `static` | 30 % | 44 % |
| Función más larga, en líneas | 128 | 424 |
| Parámetros puntero con `const` | 51 % | 44 % |
| Parámetros puntero comprobados contra `NULL` | 37 % | 10 % |
| Búferes acompañados de un tamaño | 7 % | 71 % |
| Variables de estado mutable | 3 | 0 |
| Tablas constantes | 1 | 6 |
| Conversiones explícitas por cada 100 líneas | 18 | 17 |
| `goto` | 43 | 29 |
| Llamadas sin límite (`strcpy`, `sprintf`) | 11 | 0 |
| Llamadas directas a `malloc` y `free` | 0 | 4 |

El medidor no ve el código generado por macros y su detección de tamaños se basa en nombres de parámetros. Sirve para ver órdenes de magnitud, no diferencias pequeñas.

## Lo que comparten

| Práctica | Evidencia | Qué implica para el arnés |
|---|---|---|
| Usan `goto`, previsiblemente para salidas de error | 43 y 29 usos | Confirma haber quitado el veneno de `goto` por FUN-1 |
| Hacen muchas conversiones explícitas | 17 o 18 por cada 100 líneas | Prohibir los casts haría inviable el código real: refuerza FUN-1 |
| Casi no llaman directamente al asignador | 0 y 4 llamadas | Apoya la candidata VID-2 |
| Tienen poco estado global | 3 y 0 variables | Apoya EST-2 y EST-3 |
| Compilan con avisos estrictos | Sus archivos de construcción lo muestran | Son la base común, y explican parte de la cercanía con el arnés |

## En qué discrepan

Algunas diferencias pueden ser grupos de prácticas que no conviene mezclar, según [FUN-5](premisas_c-harness.md).

| Tema | cJSON | lz4 | Lectura |
|---|---|---|---|
| Búferes | Cadenas terminadas en `'\0'` | Puntero más tamaño | Parece depender del dominio. Un puntero con tamaño es más fácil de comprobar |
| Comprobar `NULL` | A menudo, en la API | Rara vez | lz4 parece trasladar más responsabilidad a quien llama. No es una regla universal |
| Funciones sin límite | Las usa, al menos una vez tras una comprobación | No las usa | Es un fallo contenido según FUN-3: que el veneno lo rechace es la conducta correcta, aunque no haya un error demostrado |
| Longitud de funciones | Moderada | Una de 424 líneas | Un límite de longitud chocaría con código crítico en velocidad |

## Lo que ser pulido no garantiza

lz4 forma punteros fuera de los objetos a los que pertenecen, algo que la norma de C no permite, y los compara después. Funciona con memoria plana y tiene años de uso, pero su seguridad depende de esa orquestación. Es un fallo contenido, no una buena práctica. El detalle está en [AUDITORIA-LZ4.md](AUDITORIA-LZ4.md).

## Límites

- **Dos programas no bastan.** Hace falta una muestra mayor, y sobre todo incluir proyectos de otros tipos.
- **Sesgo de supervivencia.** Solo se miran programas que triunfaron. Sin comparar con programas que fallaron, no se sabe qué prácticas causan la calidad.
- **Causa y efecto.** Que un programa pulido use algo no demuestra que eso lo haga mejor.
- **Sin ejecución.** Ninguno de los dos se ejecutó con sanitizadores en esta medición.

## Qué sigue

Las reglas candidatas INT-4 y VID-2 y los grupos GRP-1 a GRP-3 están en las [premisas](premisas_c-harness.md). Antes de aceptarlas hace falta ampliar la muestra, por ejemplo con zlib, y medir su coste en rendimiento según FUN-6.
