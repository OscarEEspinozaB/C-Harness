// premisa: MEM-8 tipo: evasion
int main(void) {
  int *p = calloc(1, sizeof *p);
  if (!p) return 1;
  uintptr_t d = (uintptr_t)p;
  free(p);
  return *(int *)d;
}
