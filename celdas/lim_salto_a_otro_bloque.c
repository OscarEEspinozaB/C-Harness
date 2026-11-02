// premisa: LIM tipo: evasion
int main(void) {
  char *a = calloc(64, 1);
  char *b = calloc(64, 1);
  if (!a || !b) { free(a); free(b); return 1; }
  size_t d = (size_t)((uintptr_t)b - (uintptr_t)a);
  a[d] = 'x';   /* escribe en b a través de a */
  int r = b[0] == 'x' ? 0 : 3;
  free(a); free(b);
  return r;
}
