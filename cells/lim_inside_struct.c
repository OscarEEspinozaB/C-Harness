// premise: LIM-1 kind: evasion
// reason: outside object bounds
struct account { char name[4]; int balance; };
int main(int argc, char **argv) {
  (void)argv;
  struct account c = { "a", 0 };
  char *p = c.name;
  p[(size_t)argc + 3] = 7;   /* overwrites the balance without leaving the object */
  return 0;
}
