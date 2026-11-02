// premise: VID-1 kind: violation
// reason: use-after-free
int main(void) { int *p = malloc(sizeof *p); free(p); free(p); return 0; }
