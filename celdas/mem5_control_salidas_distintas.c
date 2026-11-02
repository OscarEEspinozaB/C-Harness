// premisa: MEM-5 tipo: control
static void dos(int *a, int *b) { *a = 1; *b = 2; }
int main(void) { int x = 0; int y = 0; dos(&x, &y); return x + y - 3; }
