# C-Harness

**¿Puede un arnés de herramientas convertir C en un lenguaje seguro?** Este proyecto busca responder a esa pregunta midiendo, no opinando. Cada afirmación se apoya en un programa pequeño, una *celda*, que el arnés debe rechazar o aceptar.

## Estado actual

Experimento de viabilidad. **C-Harness no hace seguro el código C**: busca hacer más seguro su desarrollo y declarar con claridad lo que no puede garantizar. No debe usarse todavía para proteger nada.

Dos premisas guían el proyecto:

- **C-Harness pasa por C-Harness.** El verificador está escrito en C y debe superar sus propias reglas.
- **El arnés vive con C tal como es.** No intenta construir otro lenguaje dentro de C.

Última medición (2 de noviembre de 2026, GCC 14.2): de 52 programas incorrectos, el arnés detuvo 43; quedan 3 fallos pendientes y 6 límites declarados. Aceptó los 9 programas válidos de control.

El análisis completo está en [docs/VIABILIDAD.md](docs/VIABILIDAD.md).

## Reproducir la medición

Necesitas Debian 13 o equivalente. [instalar-herramientas.sh](instalar-herramientas.sh) instala todo; para medir bastan GCC 14, `binutils`, Bash y libclang 19:

```sh
./correr.sh          # mide todas las celdas
./autoverificar.sh   # pasa el verificador por su propio arnés
```

`correr.sh` termina con error mientras alguna celda falle, y hoy es así.

La tabla indica en qué etapa se detuvo cada celda:

| Marca | Significado |
|---|---|
| `C` | Compilación: avisos convertidos en errores, analizador e identificadores envenenados |
| `V` | Verificador: `ch_verificar` encontró un incumplimiento de sus reglas |
| `S` | Símbolos: `nm` encontró estado mutable prohibido o una función externa fuera de la lista |
| `E` | Ejecución: AddressSanitizer, UBSan o LeakSanitizer abortaron, o el programa terminó con un código distinto de cero |
| `-` | Nada lo detuvo |

## Organización

| Ruta | Contenido |
|---|---|
| [docs/VIABILIDAD.md](docs/VIABILIDAD.md) | Qué funcionó, qué no y qué sigue |
| [docs/PREMISAS.md](docs/PREMISAS.md) | Premisas del proyecto: parten de Zymbol y evolucionan con el trabajo |
| [arnes/](arnes/) | Banderas, veneno de identificadores, lista de símbolos y etapas |
| [verificador/](verificador/) | El verificador en C |
| [celdas/](celdas/) | Programas de prueba: controles, violaciones y evasiones |
| [resultados/](resultados/) | Mediciones fechadas |

## Licencia

[MIT](LICENSE).
