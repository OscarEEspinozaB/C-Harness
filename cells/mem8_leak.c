// premise: VID-1 kind: violation
// reason: memory leaks
int main(void) { volatile int *p = malloc(64); if (!p) return 1; p = NULL; return 0; }
