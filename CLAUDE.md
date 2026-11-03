# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

C-Harness es un experimento: un arnés de herramientas para desarrollar en C de forma más segura, sin dejar de escribir C. Mide sus afirmaciones con programas pequeños (*celdas*) y auditando proyectos reales. Las reglas de fondo están en [docs/premisas_c-harness.md](docs/premisas_c-harness.md), y el estado de la evidencia, en [docs/VIABILIDAD.md](docs/VIABILIDAD.md).

## Idiomas

- **Código en inglés:** identificadores, comentarios, mensajes, tablas que generan los scripts y nombres de archivos y directorios.
- **Documentación en español:** README y `docs/`. Los nombres de código se citan tal cual.
- **Mensajes de commit:** hasta ahora, en español.

## Comandos

Requiere Debian 13 o equivalente, con GCC 14, `binutils`, libclang 19 en `/usr/lib/llvm-19` (ruta fijada en `harness/stages.sh`) y CBMC. `install-tools.sh` instala todo. No hay Makefile: los scripts construyen `bin/ch_verify` cuando lo necesitan.

```sh
./run.sh          # mide todas las celdas; termina con error si alguna falla
./self-check.sh   # pasa el verificador por su propio arnés, y comprueba una mutación
./audit/audit.sh cjson|lz4|zlib        # auditoría estática: no ejecuta el proyecto
./audit/audit.sh cjson|lz4|zlib --run  # añade la ejecución con sanitizadores
```

Medir una sola celda (imprime la etapa y el motivo):

```sh
bash -c '. harness/stages.sh; tmp=$(mktemp -d); trap "rm -rf \"$tmp\"" EXIT; export ASAN_OPTIONS=detect_leaks=1; build_verifier && measure_harness cells/mem8_leak.c; cat "$tmp/reason"'
```

Aplicar solo el verificador: `bin/ch_verify archivo.c -Iharness -include ch_headers.h`. Termina con 0 si no hay hallazgos, 1 si los hay y 2 si no pudo analizar.

Para registrar una medición, guarda la salida de `run.sh` en un archivo nuevo de `results/` con fecha y sufijo. Las mediciones y nombres antiguos de ese directorio son históricos: no se reescriben.

## Arquitectura

**Etapas.** `measure_harness`, en `harness/stages.sh`, pasa cada archivo por una secuencia y se detiene en la primera que lo rechaza. Bash solo coordina; deciden las herramientas:

| Etapa | Qué decide | Dónde |
|---|---|---|
| `C` | GCC con avisos como errores, `-fanalyzer` y veneno de identificadores | `harness/flags.sh`, `harness/ch_poison.h` |
| `V` | Reglas propias CONST-1, ALIAS-1, STATE-1 y NAME-1 | `verifier/ch_verify.c` |
| `S` | Estado escribible (`objdump`) y símbolos externos fuera de la lista (`nm`) | `harness/allowed_symbols.txt` |
| `B` | Búsqueda acotada con CBMC; solo si hay `main` | `bounded_search` |
| `E` | Ejecución con ASan, UBSan y LSan; solo si hay `main` | `harness/flags.sh` |

- **Veneno:** `ch_poison.h` se inyecta con `-include`, carga primero las cabeceras estándar de `ch_headers.h` y luego envenena identificadores. Una cabecera que no esté ahí se incluye después del veneno y suele fallar; este es el motivo habitual de los errores en cabeceras POSIX.
- **CONST-1:** no modela qué es escribir. Inserta `const` en el parámetro y sus redeclaraciones, reanaliza con libclang y comprueba si aumentan los errores. Por eso el verificador se detiene si el archivo tiene otros errores: una función sin declarar ocultaría escrituras.
- **Aviso de `argc`:** CBMC se repite con `argc` acotado. Un fallo que desaparece pasa a ser aviso, no rechazo.

**Celdas (`cells/*.c`).** Cada una declara su tipo en la primera línea, y las de rechazo, su motivo en la segunda:

```c
// premise: INT-1 kind: violation
// reason: CONST-1
```

| `kind` | Veredicto correcto |
|---|---|
| `control` | Programa válido: ninguna etapa lo detiene |
| `violation`, `evasion` | Se detiene y el motivo contiene el texto de `reason`. Si se detiene por otra razón, el veredicto es `**REASON**` |
| `limit` | Lo que C permite por diseño: no se detiene. Si llega a detenerse, el veredicto es `**REVIEW**` |

Los prefijos de los nombres, como `mem5_`, vienen de identificadores antiguos. El campo válido es `premise:`.

**Autoverificación.** `self-check.sh` pasa `ch_verify.c` por todas las etapas con opciones propias, como `clang-c/Index.h` y los símbolos `clang_*`. Después quita un `const` con `sed` y exige que se detecte. Al cambiar la firma de `report_const`, hay que actualizar ese `sed`. CBMC entra por `verifier/cbmc_entry.c`, que da un `argv` válido y modela `clang_getCString`; solo alcanza alrededor del 16 % del verificador.

**Auditorías.** `audit/audit.sh` mide cada etapa sin detenerse en el primer error, sobre proyectos descargados en `bin/audit/` y fijados por commit; nunca se copian al repositorio. Añadir un proyecto requiere una entrada en su `case`, `audit/<nombre>_cbmc_entry.c` y `audit/<nombre>_driver.c`. `audit/practices.py` es un medidor exploratorio en Python que no forma parte del arnés.

## Reglas del proyecto

- **Premisas:** las cambia su autor, a propósito, en un commit que no cambie nada más. Actualiza *Medido* y el registro por separado de cualquier cambio de código. Nunca debilites una premisa ni reclasifiques una celda como `limit` para que algo pase.
- **FUN-1:** el arnés no construye otro lenguaje dentro de C. Prohibir una construcción debe justificarse en un comentario de `ch_poison.h`.
- **FUN-2:** el verificador debe superar su propio arnés. Si el analizador da un falso positivo sobre él, simplifica el código; no lo silencies.
- **FUN-3 y FUN-4:** un programa estudiado es evidencia, no modelo. Un hallazgo en él se revisa antes de atribuirlo: varios resultaron ser defectos del arnés.
- **Rechazos y fallos de CBMC:** un rechazo solo vale si es por el motivo esperado. En código real, un fallo de `pointer relation` puede invalidar las lecturas posteriores del mismo camino.
- **Cifras:** los resultados de las celdas son pruebas de regresión, no una tasa de detección. No los presentes como tal.
- **Código descargado:** `--run` compila y ejecuta código de terceros. Requiere la autorización explícita del autor.
- **Corrección de defectos:** cada defecto del arnés se corrige junto con una celda que lo vigile. Al renombrar, `run.sh` debe dar los mismos números.
