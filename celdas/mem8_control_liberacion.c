// premisa: MEM-8 tipo: control
int main(void) { int *p = malloc(sizeof *p); if (!p) return 1; *p = 0; int r = *p; free(p); return r; }
