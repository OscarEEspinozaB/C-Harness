// premisa: TIP tipo: evasion
// motivo: outside object bounds
/* Una dirección inventada, no derivada de un objeto vivo. */
int main(int argc, char **argv) {
  (void)argv;
  int x = 0;
  uintptr_t dir = (uintptr_t)&x + (uintptr_t)argc * 64;
  if (argc > 5) *(int *)dir = 1;
  return x;
}
