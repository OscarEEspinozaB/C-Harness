// premise: LIM-1 kind: evasion
// reason: outside object bounds
int main(void) {
  char *a = calloc(64, 1);
  char *b = calloc(64, 1);
  if (!a || !b) { free(a); free(b); return 1; }
  size_t d = (size_t)((uintptr_t)b - (uintptr_t)a);
  a[d] = 'x';   /* writes to b through a */
  int r = b[0] == 'x' ? 0 : 3;
  free(a); free(b);
  return r;
}
