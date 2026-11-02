# Premisas

Estas premisas parten del proyecto paralelo [Zymbol](https://github.com/zymbol-lang), cuyo documento `PREMISES.md` declara un modelo de memoria para un lenguaje nuevo. Aquí se adaptan a C.

**Son un punto de partida, no un calco.** El autor del proyecto las aceptó como base el 2 de noviembre de 2026, con la intención de eliminarlas, depurarlas o cambiarlas según lo que enseñe el trabajo. C-Harness no tiene que cumplir las premisas de Zymbol al pie de la letra: su objetivo es un arnés que permita desarrollar en C de forma más segura.

Del método de Zymbol se conserva lo siguiente:

- Una premisa dice lo que debe ser verdad; la medición dice lo que ocurre hoy. La distancia entre ambas es el trabajo pendiente.
- Una premisa no se debilita para que una implementación pase.
- Una premisa solo la cambia su autor, a propósito, en un commit que no cambie nada más.
- Una celda en rojo no es motivo para debilitar una premisa.

## AUT-1. C-Harness es el primer programa que pasa por C-Harness

**Original**: «este arnes de seguridad que permita un desarrollo mas seguro. El cual debe de reflejarse desde la construccion de este mismo C-Harness osea que el primer programa que debe paras C-Harness es el mismo.»

**Normativa**: El arnés está escrito en el subconjunto de C que él mismo impone y se verifica a sí mismo con sus propias reglas. Toda excepción que necesite debe estar declarada y debe poder medirse.

**Medido** (2 de noviembre de 2026): **no se sostiene.** La parte que decide, [correr.sh](../correr.sh), está escrita en Bash, y todavía no existe ningún verificador en C.

**Sostenida por**: nada todavía.

## BASE-1. El arnés vive con C tal como es

**Original**: «estas premisas cambiaran en zymbol(La construccion de un nuevo lenguaje)era las bases del lenguaje aca debemos vivir con las limitantes y diseños del como C funciona y fue construido.»

**Normativa**: Las premisas de Zymbol fueron las bases de un lenguaje nuevo; C ya existe. C-Harness no intenta construir otro lenguaje dentro de C. Cada premisa se expresa con los diseños propios de C y declara lo que no puede garantizar por cómo está construido. Prohibir una construcción de C es una decisión que debe justificarse, no la respuesta por defecto.

**Medido** (2 de noviembre de 2026): **sin medir.** La tabla siguiente y las vías de cierre de [VIABILIDAD.md](VIABILIDAD.md) se escribieron antes de esta premisa y deben revisarse contra ella.

**Sostenida por**: nada todavía.

## Qué premisas aplican

| Premisa de Zymbol | En C | Medido hoy | Mecanismo o carencia |
|---|---|---|---|
| MEM-1. Constantes inmutables | Aplica | Se sostiene en las celdas | `const`, `-Wcast-qual`, analizador |
| MEM-2. Variable visible solo en su ámbito | Aplica | Se sostiene en las celdas | `nm`: ningún estado estático en el archivo de `main` |
| MEM-3. El módulo posee su entorno | Aplica; módulo = unidad de traducción | Se sostiene en las celdas | `nm`: ninguna variable exportada |
| MEM-4. El estado del módulo se escribe desde dentro | Aplica | **No se sostiene** | Una función puede devolver un puntero al estado |
| MEM-5. El cruce se declara en ambos extremos | Aplica | **Parcial** | `ch_sal_int` lo exige, pero los punteros crudos y la doble salida siguen permitidos |
| MEM-6. Dos niveles de entorno | Parcial; C no tiene lambdas | Se sostiene en las celdas | Ámbito de bloque; funciones anidadas prohibidas |
| MEM-7. Un nombre, una cosa | Aplica | **Parcial** | `-Wshadow` no ve una variable local que oculta a una función |
| MEM-8. Liberación automática o manual | Solo manual | **No se sostiene** | Los detectores no cubren un puntero colgante tras reutilizarse el bloque |
| COL-1 a COL-7. Colecciones | No aplican: C no tiene colecciones | No medido | Una biblioteca podría retomarlas |
| SYM-1 a SYM-8. Sistema de signos | No aplican: son reglas de diseño de Zymbol | No aplica | |
| AGT-1. Importar no ejecuta nada | Aplica | Se sostiene en las celdas | Veneno de atributos |
| AGT-2. El módulo declara su superficie | Aplica | No medido | `-Wmissing-prototypes` lo apoya |
| AGT-3. Programa alcanzable conocido de antemano | Parcial | Parcial | Veneno de `dlopen` y `dlsym`; faltan punteros a función |
| AGT-4. Superficie de efectos cerrada | Aplica | **No se sostiene** | Basta con declarar a mano una función de libc |
| AGT-5. El paquete es código fuente | Fuera de alcance por ahora | No medido | |
| TYP-2. Rechazo estático de tipos | Parcial | **Parcial** | `void *` y las conversiones explícitas la anulan |

«Se sostiene en las celdas» significa que las pocas celdas actuales pasan. No es una demostración.

## Premisas nuevas que C necesita

Zymbol hereda estas garantías de su diseño. C tiene que declararlas:

| Identificador | Regla propuesta | Medido hoy |
|---|---|---|
| LIM-1 | Ningún acceso sale del objeto al que apunta | **No se sostiene** |
| TMP-1 | Ningún puntero se usa después de terminar la vida de su objeto | **No se sostiene** |
| TIP-1 | Ningún dato se reinterpreta como otro tipo | **No se sostiene** |
| UB-1 | Ningún programa aceptado tiene comportamiento indefinido | **No se sostiene** |
| ARN-1 | El código no puede desactivar el arnés desde dentro | Se sostiene frente a las tres evasiones probadas |
