# C-Harness

**¿Puede un arnés de herramientas convertir C en un lenguaje seguro?** Este proyecto busca responder a esa pregunta midiendo, no opinando. Cada afirmación se apoya en un programa pequeño, una *celda*, que el arnés debe rechazar o aceptar.

## Estado actual

Experimento de viabilidad. **C-Harness todavía no hace seguro el código C** y no debe usarse para proteger nada.

Primera medición (2 de noviembre de 2026, GCC 14.2): el arnés detuvo 40 de 51 programas incorrectos y aceptó los 5 programas válidos de control. Los 11 restantes pasaron sin detección. Dos clases de error lo explican:

- La mayoría de los fallos parecen poder cerrarse **prohibiendo construcciones del lenguaje** con un verificador sintáctico, que aún no existe.
- La validez temporal de los punteros (`mem8_tras_cuarentena`) no se cierra con estas herramientas. Exige un modelo propio de memoria.

El análisis completo está en [docs/VIABILIDAD.md](docs/VIABILIDAD.md).

## Reproducir la medición

Necesitas Linux, GCC 14, `binutils` y Bash:

```sh
./correr.sh
```

La tabla indica en qué etapa se detuvo cada celda:

| Marca | Significado |
|---|---|
| `C` | Compilación: avisos convertidos en errores, analizador e identificadores envenenados |
| `S` | Símbolos: `nm` encontró estado mutable prohibido en el objeto |
| `E` | Ejecución: AddressSanitizer, UBSan o LeakSanitizer abortaron, o el programa terminó con un código distinto de cero |
| `-` | Nada lo detuvo |

## Organización

| Ruta | Contenido |
|---|---|
| [docs/VIABILIDAD.md](docs/VIABILIDAD.md) | Qué funcionó, qué no y qué sigue |
| [docs/PREMISAS.md](docs/PREMISAS.md) | Premisas candidatas, heredadas de Zymbol y adaptadas a C |
| [arnes/](arnes/) | Banderas de compilación, veneno de identificadores y marca de salida |
| [celdas/](celdas/) | Programas de prueba: controles, violaciones y evasiones |
| [resultados/](resultados/) | Mediciones fechadas |
