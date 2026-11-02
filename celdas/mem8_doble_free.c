// premisa: MEM-8 tipo: violacion
// motivo: use-after-free
int main(void) { int *p = malloc(sizeof *p); free(p); free(p); return 0; }
