# Auditoría de cJSON v1.7.19

Esta auditoría pasa por el arnés un programa real que nadie adaptó a él: [cJSON](https://github.com/DaveGamble/cJSON), un analizador de JSON en C muy usado, de 3191 líneas en `cJSON.c`. Se analizó sin modificarlo, y no se incluye en este repositorio: [audit/audit.sh](../audit/audit.sh) descarga una versión fijada. El resultado literal está en [audit-cjson-v1.7.19.md](../results/audit-cjson-v1.7.19.md).

**Conclusión.** El arnés rechaza cJSON tal como está, pero por reglas de estilo y de política, no porque haya encontrado errores de memoria. **No se confirmó ningún defecto en cJSON.** Lo más valioso fue lo contrario de lo que se buscaba: cJSON encontró cuatro defectos en el arnés.

**Alcance.** Es una auditoría incompleta. La etapa de ejecución con sanitizadores no se ha corrido, y CBMC no llegó a ninguna conclusión. Por tanto, esto no dice que cJSON sea seguro.

## Qué encontró cada etapa

| Etapa | Resultado | Lectura |
|---|---|---|
| Compilación: veneno | 14 usos: 6 de `sprintf`, 5 de `strcpy`, 3 de `#pragma GCC diagnostic` | Infringen la política. No se revisó si alguno desborda: no se auditaron uno por uno |
| Compilación: avisos | 1: `-Wfloat-equal` en la línea 611 | Compara con `==` a propósito. El propio proyecto ya compila con muchas de estas banderas, por eso casi no hay avisos |
| Verificador | 4 de CONST-1 | Parámetros públicos que podrían ser `const`. Cambiarlos es razonable, pero altera la API de una biblioteca publicada |
| Símbolos | `sprintf` y `strcpy`; 3 variables de estado | El estado de un módulo está permitido, y por eso no detiene. Los símbolos coinciden con el veneno |
| CBMC | **No concluyó** | Agotó su tiempo límite de 100 s incluso con una entrada de 2 caracteres |
| Ejecución | **No se corrió** | Requiere ejecutar código descargado, y falta la autorización del autor del proyecto |

Se esperaba una avalancha de errores y no ocurrió. La razón probable es que cJSON es un caso favorable: lleva años en uso y se compila con banderas estrictas. Con un programa menos cuidado el resultado podría ser muy distinto, así que una sola auditoría no permite generalizar.

## Defectos del arnés que aparecieron

| Defecto | Efecto | Estado |
|---|---|---|
| CONST-1 omitía las funciones declaradas en cabeceras | Daba «0 hallazgos» sin analizar la API pública | Corregido, con celda |
| CONST-1 ignoraba las escrituras a través de un cast | Pedía `const` en `buffer` de `cJSON_PrintPreallocated`, que sí se escribe | Corregido, con celda |
| El veneno rechazaba `math.h`, `ctype.h` y otras cabeceras estándar | 28 errores ajenos al programa, en contra de BASE-1 | Corregido, con celda |
| La lista de símbolos excluía funciones estándar inofensivas como `strtod` | Rechazos sin justificación | Corregido |

## Lo que esto cambia en la viabilidad

- **Las celdas no bastaban.** Estos cuatro defectos pasaron todas las celdas existentes y solo aparecieron con un programa real.
- **CBMC no escala solo.** Funciona en celdas pequeñas, pero no concluyó con este analizador sin más preparación. En [lz4](AUDITORIA-LZ4.md) sí concluyó, aunque sus resultados necesitaron revisión.
- **Rechazar no significa encontrar errores.** Hasta ahora, en código real, el arnés señala política y estilo. Falta demostrar que detecta un error real que importe.

## Para repetirla

```sh
./audit/audit.sh cjson         # solo análisis: no ejecuta cJSON
./audit/audit.sh cjson --run   # añade la ejecución con sanitizadores
```
