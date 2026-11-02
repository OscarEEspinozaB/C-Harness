// premise: TIP kind: evasion
// reason: outside object bounds
/* An invented address, not derived from a live object. */
int main(int argc, char **argv) {
  (void)argv;
  int x = 0;
  uintptr_t addr = (uintptr_t)&x + (uintptr_t)argc * 64;
  if (argc > 5) *(int *)addr = 1;
  return x;
}
