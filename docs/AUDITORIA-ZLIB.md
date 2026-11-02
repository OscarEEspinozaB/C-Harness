# Auditoría de zlib v1.3.2

Esta auditoría pasa por el arnés [zlib](https://github.com/madler/zlib), una biblioteca de compresión en C con décadas de uso. Se analizaron sus 15 archivos `.c`, 9703 líneas, sin modificarlos; no se incluye en este repositorio. El resultado literal está en [audit-zlib-v1.3.2.md](../results/audit-zlib-v1.3.2.md).

**Conclusión.** **No se confirmó ningún defecto de memoria en zlib.** Lo más útil fue doble: tres defectos del arnés que parecían hallazgos de zlib, y un patrón que coincide con lz4 y refuerza la hipótesis de los grupos.

**Alcance.** Es incompleta: no se ejecutó con sanitizadores, CBMC no concluyó y no se revisaron uno por uno los avisos.

## Qué encontró cada etapa

| Etapa | Resultado | Lectura |
|---|---|---|
| Compilación: veneno | 0 usos en el proyecto; 105 errores en `unistd.h` y `sys/types.h` | zlib no usa funciones vetadas. Los errores vienen de cabeceras POSIX que el arnés no admite, necesarias para sus funciones `gz*` |
| Compilación: avisos | 49 `-Wcast-qual`, 26 `-Wswitch-enum`, 19 `-Wimplicit-fallthrough`, 7 de conversión y alineación, 1 fuga | Muchos más que cJSON y lz4: su configuración usa menos banderas estrictas por defecto. La fuga, en `gzwrite.c`, está sin revisar |
| Verificador | 6 de CONST-1 | Ver la nota siguiente |
| Símbolos | Sin estado escribible; `open`, `read`, `write`, `close`, `lseek` y `fcntl` fuera de la lista | Solo en las funciones `gz*`, que hacen E/S de archivos |
| CBMC | **No concluyó** | Agotó los 100 s con 2 y con 7 bytes de entrada |
| Ejecución | **No se corrió** | Falta la autorización para ejecutar código descargado |

**Nota sobre CONST-1.** El hallazgo `desc` de `gen_bitlen` cumple la letra de la regla, pero la función sí modifica datos: extrae `desc->dyn_tree` y escribe a través de ese puntero. El `const` de C es superficial. Añadirlo sería correcto para el compilador y engañoso para quien lee. Es un límite de la regla, no un error de zlib.

## Defectos del arnés que aparecieron

| Defecto | Efecto | Estado |
|---|---|---|
| Los datos `const` que contienen punteros se confundían con estado mutable | `static const char *const names[]` se habría rechazado en un `main` | Corregido, con celda: la etapa usa `objdump` |
| STATE-1 ignoraba los arreglos `const` y los destinos `const` | Marcaba `lenfix` y `distfix`, que no dan permiso de escritura | Corregido, con dos celdas |
| El verificador toleraba una función sin declarar | Pedía `const` en `buf` de `gz_load`, que `read()` sí escribe | Corregido, con celda: ahora no analiza el archivo |

El último también revela un error de la auditoría: hacía falta definir `HAVE_UNISTD_H`, como hace el sistema de construcción de zlib.

## Observaciones

- **Tablas precalculadas.** zlib tiene 18 tablas constantes y ningún estado escribible por defecto. Es un patrón *bueno* que comparte con lz4.
- **Asignación delegada.** El núcleo de compresión reserva memoria mediante funciones que da quien llama. Las llamadas directas se concentran en `zutil.c` y en las funciones `gz*`.
- **`const` retirado por una cast.** `strm->msg = (z_const char *)"incorrect header check"` guarda un literal en un `char *`, porque por defecto `z_const` está vacío. Compilar con `ZLIB_CONST` lo evita. Es un *fallo contenido*: nada impide escribir a través de ese puntero salvo la convención. Se revisaron dos de los 49 avisos, y 42 están en `inflate.c`, `infback.c` y `zutil.c`.
- **Función muy larga.** `inflate` y su máquina de estados dominan el recuento, con la función más larga de las tres auditorías: 680 líneas. Queda abierto si eso es un problema o un coste razonable de velocidad.

## Para repetirla

```sh
./audit/audit.sh zlib         # solo análisis: no ejecuta zlib
./audit/audit.sh zlib --run   # añade la ejecución con sanitizadores
```
