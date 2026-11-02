// premisa: MEM-8 tipo: control
int main(void) {
  int r = 1;
  int *a = malloc(sizeof *a);
  if (!a) goto fin;
  int *b = malloc(sizeof *b);
  if (!b) goto suelta_a;
  *a = 0;
  *b = 0;
  r = *a + *b;
  free(b);
suelta_a:
  free(a);
fin:
  return r;
}
