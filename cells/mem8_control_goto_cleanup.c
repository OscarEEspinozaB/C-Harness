// premise: MEM-8 kind: control
int main(void) {
  int r = 1;
  int *a = malloc(sizeof *a);
  if (!a) goto done;
  int *b = malloc(sizeof *b);
  if (!b) goto free_a;
  *a = 0;
  *b = 0;
  r = *a + *b;
  free(b);
free_a:
  free(a);
done:
  return r;
}
