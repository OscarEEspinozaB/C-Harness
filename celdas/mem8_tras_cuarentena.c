// premisa: MEM-8 tipo: limite
/* Devuelve 3 si no logra reutilizar el bloque: el intento no valdría. */
int main(void) {
  char *viejo = malloc(1024);
  if (!viejo) return 1;
  uintptr_t d = (uintptr_t)viejo;
  free(viejo);
  for (int i = 0; i < 2000000; i++) {
    char *q = malloc(1024);
    if (!q) return 1;
    if ((uintptr_t)q == d) {
      *(char *)d = 'x';   /* puntero colgante, escribe en un bloque ajeno */
      free(q);
      return 0;
    }
    free(q);
  }
  return 3;
}
