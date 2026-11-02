// premisa: MEM-5 tipo: violacion
static void dos(int *restrict a, int *restrict b) { *a = 1; *b = 2; }
int main(void) { int x = 0; dos(&x, &x); return x - 2; }
