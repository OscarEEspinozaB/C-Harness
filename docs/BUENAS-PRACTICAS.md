# Buenas prácticas observadas en C

Este documento parte de una idea del autor del proyecto: si un programa pulido queda cerca del arnés, sus prácticas pueden enseñar qué es escribir buen C. Así las reglas nacerían de código que sobrevivió años de uso, y no solo de lo que parece correcto en teoría.

**Conclusión provisional.** La idea funciona como fuente de hipótesis y ya ha corregido decisiones del arnés. Pero «pulido» no equivale a «cerca del arnés»: dos programas muy usados coinciden en algunas prácticas, discrepan en otras e incumplen reglas estrictas de C. Con dos programas, todo lo que sigue es una **hipótesis**, no una regla.

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

## Prácticas que comparten

| Práctica | Evidencia | Qué implica para el arnés |
|---|---|---|
| Usan `goto`, previsiblemente para salidas de error | 43 y 29 usos | Confirma haber quitado el veneno de `goto` por BASE-1 |
| Hacen muchas conversiones explícitas | 17 o 18 por cada 100 líneas | Prohibir los casts haría inviable el código real: refuerza BASE-1 |
| Casi no llaman directamente al asignador | 0 y 4 llamadas | Candidata a regla: la memoria se gestiona en un solo lugar, o la decide quien llama |
| Tienen poco estado global | 3 y 0 variables | Apoya MEM-2 y MEM-4 |
| Compilan con avisos estrictos | Sus archivos de construcción lo muestran | Son la base común, y explican parte de la cercanía con el arnés |

## Prácticas en las que discrepan

| Tema | cJSON | lz4 | Lectura |
|---|---|---|---|
| Búferes | Cadenas terminadas en `'\0'` | Puntero más tamaño | Parece depender del dominio. Un puntero con tamaño es más fácil de comprobar |
| Comprobar `NULL` | A menudo, en la API | Rara vez | lz4 parece trasladar más responsabilidad a quien llama. No es una regla universal |
| Funciones sin límite | Las usa con comprobaciones | No las usa | El veneno rechaza cJSON aunque no se haya demostrado un error. Puede ser demasiado estricto |
| Longitud de funciones | Moderada | Una de 424 líneas | Un límite de longitud chocaría con código crítico en velocidad |

## Lo que ser pulido no garantiza

lz4 forma punteros fuera de los objetos a los que pertenecen, algo que la norma de C no permite, y los compara después. Con un modelo de memoria plano funciona en la práctica y tiene años de uso, pero es comportamiento indefinido técnico. Por tanto, una práctica no es buena solo porque aparezca en un programa exitoso. El detalle está en [AUDITORIA-LZ4.md](AUDITORIA-LZ4.md).

## Límites

- **Dos programas no bastan.** Hace falta una muestra mayor, y sobre todo incluir proyectos de otros tipos.
- **Sesgo de supervivencia.** Solo se miran programas que triunfaron. Sin comparar con programas que fallaron, no se sabe qué prácticas causan la calidad.
- **Causa y efecto.** Que un programa pulido use algo no demuestra que eso lo haga mejor.
- **Sin ejecución.** Ninguno de los dos se ejecutó con sanitizadores en esta medición.

## Propuestas para las premisas

Ninguna se ha aplicado: cambiar una premisa corresponde al autor.

1. **Prohibir menos, exigir más contexto.** Revisar si `strcpy` y `sprintf` deben seguir envenenadas o pasar a exigir una justificación.
2. **Nueva regla candidata:** los búferes se pasan con su tamaño. lz4 lo hace en el 71 % de los casos.
3. **Nueva regla candidata:** la asignación de memoria se concentra en un solo lugar.
4. **Ampliar la muestra** con zlib y otros proyectos estables antes de convertir cualquier hipótesis en regla.
