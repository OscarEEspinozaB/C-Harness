# C-Harness

**¿Puede un arnés de herramientas convertir C en un lenguaje seguro?** Este proyecto busca responder a esa pregunta midiendo, no opinando. Cada afirmación se apoya en un programa pequeño, una *celda*, que el arnés debe rechazar o aceptar.

## Estado actual

Experimento de viabilidad. **C-Harness no hace seguro el código C**: busca hacer más seguro su desarrollo y declarar con claridad lo que no puede garantizar. No debe usarse todavía para proteger nada.

Estas [premisas](docs/premisas_c-harness.md) guían el proyecto:

- **El arnés vive con C tal como es.** No intenta construir otro lenguaje dentro de C.
- **C-Harness pasa por C-Harness.** El verificador está escrito en C y debe superar sus propias reglas.
- **Las reglas contienen lo que hoy contienen la experiencia y el tiempo.** Que un programa haya sobrevivido no convierte sus prácticas en buenas.

Última medición (2 de noviembre de 2026, GCC 14.2): de 59 programas incorrectos, el arnés detuvo 55 y los otros 4 son límites declarados, y aceptó los 20 programas válidos de control. Son pruebas de regresión, no una tasa de detección. Las auditorías, todavía incompletas, de [cJSON](docs/AUDITORIA-CJSON.md), [lz4](docs/AUDITORIA-LZ4.md) y [zlib](docs/AUDITORIA-ZLIB.md) no confirmaron errores de memoria y encontraron siete defectos en el propio arnés. Esos programas son evidencia, no modelos: lo observado se recoge en [prácticas observadas](docs/PRACTICAS-OBSERVADAS.md).

El análisis completo está en [docs/VIABILIDAD.md](docs/VIABILIDAD.md).

## Idiomas

El código está en inglés: identificadores, comentarios, mensajes y nombres de archivo. La documentación está en español. Los nombres de código citados en los documentos se escriben tal como aparecen en el código.

## Reproducir la medición

Necesitas Debian 13 o equivalente. [install-tools.sh](install-tools.sh) instala todo; para medir bastan GCC 14, `binutils`, Bash, libclang 19 y CBMC:

```sh
./run.sh          # mide todas las celdas
./self-check.sh   # pasa el verificador por su propio arnés
```

`run.sh` termina con error si alguna celda falla.

La tabla indica en qué etapa se detuvo cada celda:

| Marca | Significado |
|---|---|
| `C` | Compilación: avisos convertidos en errores, analizador e identificadores envenenados |
| `V` | Verificador: `ch_verify` encontró un incumplimiento de sus reglas |
| `S` | Símbolos: `nm` encontró estado mutable prohibido o una función externa fuera de la lista |
| `B` | Búsqueda acotada: CBMC encontró un fallo sin ejecutar el programa |
| `E` | Ejecución: AddressSanitizer, UBSan o LeakSanitizer abortaron, o el programa terminó con un código distinto de cero |
| `-` | Nada lo detuvo |

## Organización

| Ruta | Contenido |
|---|---|
| [docs/VIABILIDAD.md](docs/VIABILIDAD.md) | Qué funcionó, qué no y qué sigue |
| [docs/PRACTICAS-OBSERVADAS.md](docs/PRACTICAS-OBSERVADAS.md) | Lo observado en programas reales, bueno y malo |
| [docs/premisas_c-harness.md](docs/premisas_c-harness.md) | Premisas del proyecto, escritas desde C, y registro de lo observado |
| [harness/](harness/) | Banderas, veneno de identificadores, lista de símbolos y etapas |
| [verifier/](verifier/) | El verificador en C |
| [cells/](cells/) | Programas de prueba: controles, violaciones y evasiones |
| [audit/](audit/) | Auditorías de programas reales |
| [results/](results/) | Mediciones y auditorías |

## Licencia

Copyright (C) 2026 OscarEEspinozaB. Este programa es software libre bajo la [GNU Affero General Public License, versión 3](LICENSE).
