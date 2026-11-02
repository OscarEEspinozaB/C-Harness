// premise: MEM-8 kind: violation
// reason: use-after-free
int main(void) { int *p = malloc(sizeof *p); if (!p) return 1; free(p); return *p; }
