// premisa: MEM-5 tipo: evasion
static void sube(int *p) { *p += 1; }
int main(void) { int x = 0; sube(&x); return x - 1; }
