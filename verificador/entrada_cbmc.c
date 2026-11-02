/* entrada_cbmc.c: punto de entrada para analizar el verificador con CBMC.
 * El modelo de CBMC deja sin definir el contenido de argv; C garantiza que
 * argv[i] es una cadena válida si i < argc. Aquí argv tiene esa forma.
 * CBMC no entra en libclang: sus funciones devuelven valores sin restricción. */
#define main ch_programa
#include "ch_verificar.c"
#undef main

int main(void) {
  char programa[] = "ch_verificar";
  char archivo[] = "entrada.c";
  char *argumentos[] = { programa, archivo, NULL };
  return ch_programa(2, argumentos);
}
