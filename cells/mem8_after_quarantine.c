// premise: VID-1 kind: limit
/* Returns 3 if the block is never reused: the attempt would not count. */
int main(void) {
  char *stale = malloc(1024);
  if (!stale) return 1;
  uintptr_t d = (uintptr_t)stale;
  free(stale);
  for (int i = 0; i < 2000000; i++) {
    char *q = malloc(1024);
    if (!q) return 1;
    if ((uintptr_t)q == d) {
      *(char *)d = 'x';   /* dangling pointer: writes to a block it no longer owns */
      free(q);
      return 0;
    }
    free(q);
  }
  return 3;
}
