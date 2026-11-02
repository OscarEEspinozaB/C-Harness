// premisa: MEM-5 tipo: violacion
// motivo: alias
static void dos(int *a, int *b) { *a = 1; *b = 2; }
int main(void) { int x = 0; dos(&x, &x); return x - 2; }
