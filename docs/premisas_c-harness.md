# Premisas de C-Harness

C es un lenguaje potente que confía en quien lo escribe. Esa confianza se ha pagado durante décadas con experiencia, pruebas exhaustivas y parches acumulados. C-Harness busca sustituir parte de ese coste por **reglas claras que contengan los fallos desde el principio**, sin dejar de escribir C.

Este documento dice lo que debe ser verdad. Lo que ocurre hoy está en el campo *Medido* de cada premisa, y la distancia entre ambos es el trabajo pendiente.

## Cómo leer una premisa

| Campo | Significado |
|---|---|
| **Estado** | *Declarada*: la enunció el autor. *Adoptada*: el autor la aceptó como base. *Candidata*: propuesta, aún sin aceptar |
| **Original** | Palabras textuales del autor, cuando las hay |
| **Normativa** | La regla, en una forma que se pueda medir. Es una interpretación que el autor puede corregir |
| **Medido** | Qué ocurre hoy, con fecha. Puede decir que la premisa no se sostiene |
| **Sostenida por** | Lo que se pondría en rojo si dejara de cumplirse |

Reglas de este documento:

- Una premisa la cambia su autor, a propósito, en un commit que no cambie nada más.
- Una premisa puede eliminarse, depurarse o cambiarse. No se debilita para que una implementación pase.
- Una celda en rojo no es motivo para debilitar una premisa.
- Un identificador no se reutiliza.

«Se sostiene en las celdas» significa que las celdas actuales pasan. No es una demostración.

---

## 1. Fundamentos

### FUN-1. El arnés vive con C tal como es

**Estado**: Declarada.

**Original**: «estas premisas cambiaran en zymbol(La construccion de un nuevo lenguaje)era las bases del lenguaje aca debemos vivir con las limitantes y diseños del como C funciona y fue construido.»

**Normativa**: C-Harness no construye otro lenguaje dentro de C. Cada premisa se expresa con los diseños propios de C y declara lo que no puede garantizar por cómo está construido. Prohibir una construcción de C es una decisión que debe justificarse, no la respuesta por defecto.

**Medido** (2 de noviembre de 2026): se aplica. Cada prohibición declara su razón en `harness/ch_poison.h`, y lo que C permite por diseño queda como celda de tipo `limit`.

**Sostenida por**: revisión. Ningún programa demuestra que una prohibición esté justificada.

### FUN-2. C-Harness es el primer programa que pasa por C-Harness

**Estado**: Declarada.

**Original**: «este arnes de seguridad que permita un desarrollo mas seguro. El cual debe de reflejarse desde la construccion de este mismo C-Harness osea que el primer programa que debe paras C-Harness es el mismo.»

**Normativa**: El arnés está escrito en el subconjunto de C que él mismo impone y se verifica con sus propias reglas. Toda excepción está declarada y se puede medir.

**Medido** (2 de noviembre de 2026): **se sostiene en parte.** El verificador pasa todas las etapas y una mutación demuestra que el paso no es trivial. Pero Bash coordina las etapas, el verificador confía en GCC, libclang y libc, y CBMC solo alcanza el 15,7 % de su código.

**Sostenida por**: `self-check.sh`.

### FUN-3. Las reglas contienen lo que hoy contienen la experiencia y el tiempo

**Estado**: Declarada.

**Original**: «NO todo programa debe tomarse como que cumple con buenas practicas puede tener algunas programaciones de fallos de seguridad criticar pero contenidas y depuradas por mucho codigo poco seguro pero bien orquestado. […] solo el tiempo te permite tener muchos parches que van acotando y depurando los errores por contencion o bajo un conocimento del como estos temas pueden ser bien programados por alguien con mucha experiencia o pruebas muy exaustivas... pero es lo que queremos evitar con reglas claras que ayuden a contener estos fallos con relas que ayuden a una buena programacion en un lenguaje tan potente como C»

**Normativa**: Que un programa haya sobrevivido no convierte sus prácticas en buenas. Se llama **fallo contenido** a una construcción peligrosa cuya seguridad depende de una condición que ninguna regla ni herramienta puede relacionar con ella: una comprobación situada en otro lugar, una convención o el conocimiento de quien la escribió. C-Harness prefiere una forma cuya seguridad se pueda comprobar sobre una forma que solo es segura porque está bien orquestada.

**Medido** (2 de noviembre de 2026): identificados en cJSON y lz4; ver el [registro](#6-registro-de-lo-observado). Todavía no hay una etapa que los distinga de forma general.

**Sostenida por**: nada todavía.

### FUN-4. Se incorpora lo que se cierra, por malo o por bueno

**Estado**: Declarada.

**Original**: «ir incorporando lo que cierra sea por malo o por bueno»

**Normativa**: Todo programa estudiado es evidencia, nunca modelo. Cada observación se registra con su veredicto, *bueno*, *malo contenido*, *malo* o *abierto*, y con la evidencia que lo apoya. Solo se convierte en regla cuando se ha cerrado con evidencia suficiente.

**Medido** (2 de noviembre de 2026): tres programas estudiados, de forma parcial. Ninguna observación ha llegado aún a regla por sí sola.

**Sostenida por**: el [registro](#6-registro-de-lo-observado).

### FUN-5. Hay grupos de cosas buenas que no se mezclan

**Estado**: Declarada.

**Original**: «me imagino que en algunos caos tendremos grupos de cosas buenas que no se deben de mezcla con otros grupos de cosas buenas porque saldria algo mediocre.»

**Normativa**: Una práctica se evalúa dentro de su grupo. Un programa o módulo declara a qué grupo pertenece, y mezclar prácticas de grupos incompatibles es un defecto aunque cada práctica sea buena por separado.

**Medido** (2 de noviembre de 2026): solo hipótesis; ver los [grupos](#5-grupos-que-no-se-mezclan). No se ha medido que la mezcla sea peor.

**Sostenida por**: nada todavía.

### FUN-6. El coste en rendimiento se mide

**Estado**: Declarada.

**Original**: «ahora como esto afectaria la performance de un programa es lo otro que tendremos que medir a largo plazo.»

**Normativa**: Cada regla declara dónde cuesta: al compilar, durante la verificación o en el programa final. Ninguna regla que cambie el programa final se da por gratuita sin medirla. Un coste medido alto no invalida la regla por sí solo: es información para decidir.

**Medido** (2 de noviembre de 2026): **sin medir.** Se espera que las reglas de compilación y de verificación no cuesten nada en el programa final, pero nadie lo ha comprobado. Los sanitizadores se usan solo en la verificación.

**Sostenida por**: nada todavía.

---

## 2. Estado e interfaces

### EST-1. Una constante no cambia

**Estado**: Adoptada.

**Normativa**: Un objeto `const` no se escribe, tampoco quitándole `const` con una conversión.

**Medido** (2 de noviembre de 2026): se sostiene en las celdas, con `const`, `-Wcast-qual` y el analizador.

**Sostenida por**: `mem1_reassign_constant`, `mem1_cast_away_const`, `mem1_cast_away_const_via_integer`.

### EST-2. No hay estado mutable compartido a la vista

**Estado**: Adoptada.

**Normativa**: Ninguna unidad de traducción exporta variables. El archivo que define `main` no tiene estado mutable con duración estática, ni siquiera `static` local. Los valores cruzan las funciones como parámetros. C impone excepciones como `errno` o `stdout`, que se declaran como límite.

**Medido** (2 de noviembre de 2026): se sostiene en las celdas, con `objdump`, que distingue los datos `const` que contienen punteros.

**Sostenida por**: `mem2_file_variable`, `mem2_static_local`, `mem3_exported_variable`.

### EST-3. El estado de un módulo solo lo escribe el módulo

**Estado**: Adoptada.

**Normativa**: El estado `static` de un módulo no sale de él con permiso de escritura.

**Medido** (2 de noviembre de 2026): **parcial.** La regla STATE-1 detiene la dirección directa. La dirección que pasa por una variable intermedia es un límite declarado.

**Sostenida por**: `mem4_pointer_to_state`, `mem4_static_array`, `mem4_output_via_parameter`. Límite: `mem4_indirect_pointer`.

### EST-4. Un nombre vive en su bloque

**Estado**: Adoptada.

**Normativa**: Un nombre declarado en un bloque no se usa fuera de él, y no hay funciones anidadas.

**Medido** (2 de noviembre de 2026): se sostiene en las celdas.

**Sostenida por**: `mem6_block_does_not_leak`, `mem6_nested_function`.

### INT-1. La firma dice qué se puede modificar

**Estado**: Adoptada.

**Normativa**: `const T *` es entrada y `T *` es salida. Un parámetro `T *` que la función no escribe debe ser `const T *`. En la llamada, una salida se pasa como `&x` siempre que sea posible.

**Medido** (2 de noviembre de 2026): **parcial.** La regla CONST-1 exige el `const`, también en las cabeceras del proyecto. Pasar una variable puntero no muestra `&` en la llamada y es un límite declarado. CONST-1 tampoco ve las escrituras a través de un puntero guardado en una estructura, como en `gen_bitlen` de zlib.

**Sostenida por**: `mem5_output_not_written`, `mem5_prototype_not_written`, `mem5_header_prototype_not_written`. Límite: `mem5_output_via_pointer_variable`.

### INT-2. Una variable no entra dos veces como salida

**Estado**: Adoptada.

**Normativa**: Una llamada no recibe la misma variable en dos parámetros de salida. Una vez como salida y otra como lectura es válido.

**Medido** (2 de noviembre de 2026): **parcial.** La regla ALIAS-1 detiene `&x` directos en llamadas a funciones con nombre.

**Sostenida por**: `mem5_same_output_twice`, `mem5_same_output_restrict`.

### INT-3. Un nombre designa una sola cosa

**Estado**: Adoptada.

**Normativa**: Dentro de un archivo, una variable local o un parámetro no se llama como una función de ese archivo ni oculta otra variable.

**Medido** (2 de noviembre de 2026): se sostiene en las celdas, con `-Wshadow` y la regla NAME-1.

**Sostenida por**: `mem7_local_shadow`, `mem7_duplicate_parameters`, `mem7_variable_named_as_function`, `mem7_parameter_named_as_function`.

### INT-4. Un búfer viaja con su tamaño

**Estado**: Candidata.

**Normativa**: Una función que recibe un búfer recibe también su tamaño. Las cadenas terminadas en `'\0'` son una excepción que debe declararse.

**Medido** (2 de noviembre de 2026): sin regla. Observado con una heurística en el 72 % de los búferes de zlib, el 71 % de los de lz4 y el 7 % de los de cJSON.

**Sostenida por**: nada todavía.

### INT-5. Un módulo declara lo que ofrece

**Estado**: Adoptada.

**Normativa**: Una función no `static` tiene un prototipo visible antes de su definición.

**Medido** (2 de noviembre de 2026): `-Wmissing-prototypes` lo exige. No tiene celda propia.

**Sostenida por**: nada todavía.

---

## 3. Memoria y comportamiento

### LIM-1. Un acceso no sale de su objeto

**Estado**: Adoptada.

**Normativa**: Ninguna lectura o escritura sale del objeto al que pertenece el puntero, y no se forman punteros fuera de él más allá de la posición siguiente al último elemento.

**Medido** (2 de noviembre de 2026): **parcial.** Sanitizadores y CBMC lo detienen en las celdas, solo en los caminos y límites que exploran. Los fallos de CBMC en código real requieren revisión.

**Sostenida por**: `lim_constant_index`, `lim_computed_index`, `lim_adjacent_heap`, `lim_jump_to_other_block`, `lim_pointer_arithmetic`, `lim_inside_struct`.

### VID-1. Un puntero no se usa después de la vida de su objeto

**Estado**: Adoptada.

**Normativa**: Ningún puntero se usa tras liberar o terminar la vida de su objeto. La memoria se libera exactamente una vez. En C, `free` no invalida las copias de un puntero, y por eso esta premisa no se puede garantizar por completo.

**Medido** (2 de noviembre de 2026): **no se sostiene.** Los casos directos caen, pero un puntero colgante que escribe en un bloque reutilizado pasa sin detección. Es el límite más serio.

**Sostenida por**: `mem8_use_after_free`, `mem8_opaque_use_after_free`, `mem8_unexecuted_path`, `mem8_double_free`, `mem8_leak`. Límite: `mem8_after_quarantine`.

### VID-2. La asignación de memoria está en un solo lugar

**Estado**: Candidata.

**Normativa**: Las llamadas a `malloc`, `realloc` y `free` se concentran en un punto del módulo, o la gestión se delega en quien llama.

**Medido** (2 de noviembre de 2026): sin regla. cJSON hace 0 llamadas directas en su código de funciones, y lz4, 4. En zlib, las directas están en `zutil.c` y en las funciones `gz*`, y el núcleo las delega.

**Sostenida por**: nada todavía.

### TIP-1. Un dato no se reinterpreta sin decirlo

**Estado**: Adoptada.

**Normativa**: Las conversiones implícitas no pierden información ni cambian el signo. Una reinterpretación se escribe de forma explícita. `void *` y las conversiones son parte de C y no se prohíben, pero borran garantías.

**Medido** (2 de noviembre de 2026): **parcial.** Los avisos de GCC detienen las conversiones implícitas, y CBMC, una dirección inventada. La confusión mediante `void *` es un límite declarado.

**Sostenida por**: `typ_narrowing_conversion`, `typ_undeclared`, `typ_integer_to_pointer`. Límite: `typ_void_confusion`.

### UB-1. Un programa aceptado no tiene comportamiento indefinido

**Estado**: Adoptada.

**Normativa**: Es el objetivo, no una garantía. Cada forma de comportamiento indefinido que el arnés no detecte se declara.

**Medido** (2 de noviembre de 2026): **no se sostiene en general.** Se detienen desbordamientos de enteros, división entre cero, variables sin inicializar y otros casos en las celdas. No hay un inventario de lo que falta.

**Sostenida por**: `ub_integer_overflow`, `ub_argc_overflow_within_bound`, `ub_division_by_zero`, `ub_uninitialized`, `ub_returns_local`, `ub_infinite_recursion`, `ub_data_race`.

---

## 4. Efectos y arnés

### EFE-1. Nada se ejecuta antes de `main`

**Estado**: Adoptada.

**Normativa**: Enlazar un módulo no ejecuta código propio de ese módulo.

**Medido** (2 de noviembre de 2026): se sostiene en las celdas, con el veneno de atributos.

**Sostenida por**: `agt1_constructor`, `agt1_hidden_initializer`, `agt1_initializer_in_module`.

### EFE-2. El programa alcanzable se conoce de antemano

**Estado**: Adoptada.

**Normativa**: Un programa no carga ni ejecuta código elegido durante la ejecución.

**Medido** (2 de noviembre de 2026): **parcial.** `dlopen` y `system` están envenenados. Los punteros a función son parte de C y siguen permitidos.

**Sostenida por**: `agt3_dynamic_loading`.

### EFE-3. La superficie de efectos es una lista cerrada

**Estado**: Adoptada.

**Normativa**: Las funciones externas que un programa puede usar forman una lista explícita.

**Medido** (2 de noviembre de 2026): se sostiene en las celdas, con `nm` y `harness/allowed_symbols.txt`. El verificador añade `clang_*` para sí mismo.

**Sostenida por**: `agt4_system`, `agt4_alias_declaration`, `agt4_token_paste_declaration`, `agt4_unlisted_effect`.

### ARN-1. El código no puede desactivar el arnés

**Estado**: Adoptada.

**Normativa**: Nada dentro del código verificado apaga una comprobación.

**Medido** (2 de noviembre de 2026): se sostiene frente a las tres evasiones probadas. No se ha hecho una revisión adversaria independiente.

**Sostenida por**: `evasion_silence_warning`, `evasion_own_header`, `evasion_line_marker_header`.

---

## 5. Grupos que no se mezclan

Aplican FUN-5. **Son hipótesis sin medir**, y cada una debe confirmarse o eliminarse.

| Grupo | Eje | Alternativas | Por qué la mezcla sería peor |
|---|---|---|---|
| GRP-1 | Validación | **Defensiva**: cada función pública comprueba su entrada. **Por contrato**: se valida una vez en la frontera y el interior confía | Comprobar a medias paga el coste de la primera sin dar la seguridad de ninguna: nadie sabe quién valida |
| GRP-2 | Texto y datos | **Cadenas terminadas en `'\0'`**. **Puntero con tamaño** | Convertir una y otra vez recorre los datos de nuevo y permite que el tamaño y el terminador se contradigan |
| GRP-3 | Propiedad de la memoria | **`malloc` y `free` individuales**. **Arena** liberada en bloque | Un puntero de arena pasado a `free`, o uno individual olvidado en una arena, rompe las garantías de ambas |
| GRP-4 | Detección dinámica | **AddressSanitizer con UBSan**. **Valgrind**. **ThreadSanitizer** | No se combinan en una misma compilación: necesitan ejecuciones separadas |

GRP-4 es distinto de los demás: es una incompatibilidad conocida de las herramientas, no un juicio sobre diseño.

Hay un primer indicio para GRP-1 y GRP-2, aunque no mide si mezclar es peor. Los programas estudiados no se reparten al azar: lz4 y zlib combinan puntero con tamaño y pocas comprobaciones de `NULL` (71 y 72 % de búferes con tamaño; 10 y 9 % de punteros comprobados), mientras que cJSON combina cadenas y muchas comprobaciones (7 % y 37 %).

---

## 6. Registro de lo observado

Aplican FUN-3 y FUN-4. Los proyectos son evidencia, no modelos, y un veredicto se refiere a la construcción, no al proyecto. *Malo contenido* no afirma que exista una vulnerabilidad.

| Observación | Origen | Veredicto | Consecuencia |
|---|---|---|---|
| Forma y compara punteros fuera del objeto; una comprobación posterior corta el camino | lz4, líneas 2051 y 2356 | Malo contenido | Apoya LIM-1 |
| `strcpy` protegido por una comprobación de longitud situada unas líneas antes | cJSON, línea 456 | Malo contenido | `strcpy` sigue envenenado |
| Desactiva un aviso de `-Wcast-qual` en un punto | cJSON, línea 2055 | Malo contenido | Apoya ARN-1 |
| Literal guardado en un `char *` retirando `const` con una conversión | zlib, `inflate.c`, línea 529 | Malo contenido | Apoya TIP-1 y EST-1 |
| Sin estado mutable y con tablas precalculadas constantes | lz4; zlib, 18 tablas | Bueno | Apoya EST-2 y EST-3 |
| Búfer acompañado de su tamaño | zlib, 72 %; lz4, 71 % | Bueno | Apoya INT-4 |
| Asignación delegada mediante funciones configurables | cJSON; núcleo de zlib | Bueno | Apoya VID-2 |
| `goto` hacia una salida de error | cJSON, 40 de 43; lz4, 17 de 29 | Bueno | `goto` no se envenena |
| Estado global de configuración y error | cJSON | Abierto | Permitido hoy por EST-3 |
| De 16 a 18 conversiones explícitas por cada 100 líneas | cJSON, lz4 y zlib | Abierto | No se prohíben; cada una borra garantías |
| `__attribute__` para optimizar | lz4, 3 usos | Abierto | Está envenenado, pero podría chocar con FUN-6 |
| Funciones de 424 y 680 líneas en la parte crítica | lz4; zlib, `inflate` | Abierto | Sin límite de longitud hasta medir |

---

## Apéndice: procedencia

La primera versión de estas premisas se adaptó del modelo de memoria del lenguaje [Zymbol](https://github.com/zymbol-lang). Se mantiene esta equivalencia para leer las mediciones antiguas. Los prefijos de los nombres de las celdas, como `mem5_`, también vienen de ahí.

| Identificador antiguo | Actual |
|---|---|
| BASE-1, AUT-1 | FUN-1, FUN-2 |
| MEM-1 | EST-1 |
| MEM-2, MEM-3 | EST-2 |
| MEM-4 | EST-3 |
| MEM-5 | INT-1, INT-2 |
| MEM-6 | EST-4 |
| MEM-7 | INT-3 |
| MEM-8, TMP-1 | VID-1 |
| LIM-1, TIP-1, UB-1, ARN-1 | Sin cambios |
| TYP-2 | TIP-1 |
| AGT-1, AGT-2, AGT-3, AGT-4 | EFE-1, INT-5, EFE-2, EFE-3 |

Se eliminaron por no tener sentido en C: COL-1 a COL-7 (colecciones del lenguaje), SYM-1 a SYM-8 (su sistema de signos), AGT-5 (su formato de paquetes), y las partes de MEM-6 sobre lambdas y de MEM-8 sobre liberación automática.
