// premisa: MEM-5 tipo: limite
static void sube(int *p) { *p += 1; }
int main(void) { int x = 0; int *q = &x; sube(q); return x - 1; }
