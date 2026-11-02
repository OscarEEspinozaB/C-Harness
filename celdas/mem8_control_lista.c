// premisa: MEM-8 tipo: control
struct nodo { struct nodo *sig; int v; };
int main(void) {
  struct nodo *cabeza = NULL;
  for (int i = 0; i < 4; i++) {
    struct nodo *n = malloc(sizeof *n);
    if (!n) break;
    n->sig = cabeza;
    n->v = i;
    cabeza = n;
  }
  while (cabeza) { struct nodo *s = cabeza->sig; free(cabeza); cabeza = s; }
  return 0;
}
