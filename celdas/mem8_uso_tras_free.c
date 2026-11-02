// premisa: MEM-8 tipo: violacion
// motivo: use-after-free
int main(void) { int *p = malloc(sizeof *p); if (!p) return 1; free(p); return *p; }
