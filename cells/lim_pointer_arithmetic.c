// premise: LIM kind: evasion
// reason: outside object bounds
int main(int argc, char **argv) {
  (void)argv;
  int a[8] = {0};
  int *p = a + argc;
  return *(p + 2);
}
