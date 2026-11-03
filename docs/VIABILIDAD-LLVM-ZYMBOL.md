# Viabilidad: un compilador de Zymbol en C sobre LLVM

**Conclusión.** Es viable como proyecto y no hay ningún bloqueo técnico a la vista, pero todavía no se ha escrito nada del compilador. Lo único medido son las herramientas de LLVM con tres programas mínimos. Las dificultades reales son el runtime y que C-Harness aún no sabe medir un proyecto de varios archivos.

[Zymbol](https://github.com/zymbol-lang) es un lenguaje con intérpretes propios. Este documento vive en C-Harness porque el compilador sería el primer programa propio y grande escrito bajo el arnés.

## Decisiones del autor

- **El destino es LLVM.** No se usa GCC como destino y no se transpila a C: Zymbol debe tener sus propias características.
- **El primer compilador se escribe en C.**
- **Debe llegar a compilarse a sí mismo**, a partir de una versión básica.

## Recomendación pendiente de confirmar: IR como texto

El compilador escribiría archivos `.ll` en lugar de enlazar la API en C de LLVM. Sigue siendo compilación a LLVM, no transpilación: LLVM IR es el destino normal de un front-end.

| | IR como texto | API en C de LLVM |
|---|---|---|
| Arnés | C estándar, sin dependencias nuevas | Hay que autorizar sus símbolos y sus cabeceras |
| Versión en Zymbol | Solo necesita manejar texto y archivos | Zymbol necesitaría llamar a funciones de C |
| Cambios de versión de LLVM | El formato puede cambiar: hay que fijar la versión | Más estable, aunque también cambia |
| Velocidad de compilación | Algo menor, porque LLVM debe leer el texto | Mayor |

Para un compilador de arranque pesan más las dos primeras filas.

## Qué se midió

El 2 de noviembre de 2026, con clang 19.1.7 de Debian. Son tres programas de pocas líneas: comprueban que las herramientas funcionan, no que el diseño funcione.

| Prueba | Resultado |
|---|---|
| Compilar y ejecutar un programa que imprime un texto | Funciona con `clang-19 ok.ll -o ok` |
| Generar solo el objeto | Funciona con `llc-19 -filetype=obj ok.ll -o ok.o` |
| Escribir 8 bytes después del inicio de un bloque de 4, con `clang-19 -fsanitize=address` | ASan lo detiene: `heap-buffer-overflow` |
| Devolver `i64` en una función declarada `i32` | LLVM lo rechaza al compilar |

ASan solo instrumenta las funciones con el atributo `sanitize_address`, así que el compilador tendría que emitirlo en un modo de prueba.

**Instalado:** `llc-19`, `opt-19`, `clang-19`, `llvm-as-19`, `llvm-config-19`, la API en C y las bibliotecas de ASan. **Falta:** `lld-19`. No es imprescindible, porque `clang-19` puede enlazar con el enlazador del sistema.

## El camino hasta compilarse a sí mismo

| Etapa | Escrita en | Compilada por | Produce |
|---|---|---|---|
| `zyc0` | C | GCC, dentro del arnés | El compilador de arranque |
| `zyc1` | Zymbol (`zyc.zy`) | `zyc0` | Primer compilador en Zymbol |
| `zyc2` | Zymbol (`zyc.zy`) | `zyc1` | Compilador compilado por sí mismo |
| `zyc3` | Zymbol (`zyc.zy`) | `zyc2` | Comprobación |

- **Punto fijo.** `zyc2` y `zyc3` deben generar exactamente el mismo IR. Si coinciden, el compilador ya se mantiene solo.
- **`zyc0` solo necesita un subconjunto.** Basta con lo que use `zyc.zy`. Cuanto más pequeño sea ese subconjunto, más corto es el camino.
- **`zyc0` se conserva.** Es el origen comprobable de la cadena.
- **Un testigo independiente.** `zyc.zy` también debería poder correr en los intérpretes y generar el mismo IR. Falta comprobar que el subconjunto lo permita.

`zyc` es un nombre provisional.

## Lo que ya existe en Zymbol

- **Comparación entre motores.** ZyDDT compara `zytw`, `zyvm` y `zyjs`, y añadir un motor es añadir una entrada en `engines.toml`. El compilador se probaría contra ellos.
- **Una referencia para portar.** Tamaño actual de las partes en Rust que `zyc0` tendría que cubrir, al menos en parte:

| Crate | Líneas |
|---|---:|
| `zymbol-lexer` | 4678 |
| `zymbol-parser` | 8346 |
| `zymbol-ast` | 3047 |
| `zymbol-semantic` | 13 310 |
| `zymbol-compiler` (a bytecode) | 6547 |
| **Total** | **35 928** |

Es un conteo simple de archivos `.rs`, que puede incluir pruebas. El proyecto completo suma unas 82 700 líneas, con LSP, formateador y otras herramientas que `zyc0` no necesita. El lenguaje completo es mucho más que el subconjunto inicial, así que este total es un techo, no una estimación.

## Riesgos

1. **El runtime.** Según `ARCHITECTURE.md`, los valores llevan etiqueta de tipo y usan conteo de referencias, copia al escribir y closures. En Rust, `Rc` da gran parte de esa seguridad. En C hay que construirla y probarla, y es la parte que más influye en la seguridad de los programas compilados.
2. **El arnés no cubre lo más importante todavía.** Ver la sección siguiente.
3. **Pérdida de garantías de Rust.** El arnés no reemplaza todo lo que da el compilador de Rust, y aún no ha detectado un error real en código real.
4. **Velocidad.** Si cada operación comprueba etiquetas en ejecución, compilar ganará menos de lo esperado. Hará falta inferir tipos más adelante, sin cambiar la semántica. Sin medir.
5. **Comportamiento indefinido en el IR.** LLVM IR también lo tiene, y el rechazo de un IR mal formado no lo detecta.
6. **Nombres de todos los alfabetos.** Los programas de Zymbol usan identificadores en devanagari, griego, hangul o cirílico. Hay que leerlos como UTF-8 correcto y traducirlos a símbolos válidos para el enlazador.

## Qué le falta a C-Harness

- **Modo proyecto.** `measure_harness` trabaja con un solo archivo. `check_symbols` rechaza una función de otro módulo propio salvo que se añada a `CH_EXTRA_SYMBOLS`, y CBMC y los sanitizadores solo corren si ese archivo tiene `main`.
- **Uso desde otro repositorio.** El compilador viviría en su propio repositorio.
- **Biblioteca de arenas.** Un compilador puede reservar memoria en arenas y liberarla al final, lo que reduce mucho el riesgo de punteros colgantes. Aún no existe; la vigilaría la celda `mem8_after_quarantine`.

Cuando el compilador esté en Zymbol, el arnés dejará de cubrirlo. Seguirá cubriendo el runtime mientras esté en C.

## Orden propuesto

1. Modo proyecto y biblioteca de arenas en C-Harness, con sus celdas.
2. Lexer en C, comparado con el de Rust sobre programas reales.
3. Parser y análisis semántico del subconjunto.
4. Generación de IR y runtime mínimo, y registro de `zyc0` como motor en ZyDDT.
5. Ampliar el subconjunto hasta escribir `zyc.zy`, y buscar el punto fijo.

## Preguntas abiertas

- ¿IR como texto o API de LLVM?
- ¿Qué subconjunto de Zymbol necesita `zyc.zy`?
- ¿El runtime se queda en C para siempre o se reescribe más adelante?
- ¿En qué repositorio vive el compilador?
