// premisa: MEM-8 tipo: violacion
int main(void) { volatile int *p = malloc(64); if (!p) return 1; p = NULL; return 0; }
