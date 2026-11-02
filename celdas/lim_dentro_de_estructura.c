// premisa: LIM tipo: evasion
// motivo: outside object bounds
struct cuenta { char nombre[4]; int saldo; };
int main(int argc, char **argv) {
  (void)argv;
  struct cuenta c = { "a", 0 };
  char *p = c.nombre;
  p[(size_t)argc + 3] = 7;   /* pisa saldo sin salir del objeto */
  return 0;
}
