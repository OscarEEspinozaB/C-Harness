// premise: VID-1 kind: evasion
// reason: use-after-free
int main(int argc, char **argv) {
  (void)argv;
  int *p = calloc(1, sizeof *p);
  if (!p) return 1;
  uintptr_t d = (uintptr_t)p;
  int r = 0;
  if (argc > 5) { free(p); r = *(int *)d; return r; }
  free(p);
  return r;
}
