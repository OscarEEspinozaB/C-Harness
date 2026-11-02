// premise: MEM-5 kind: violation
// reason: ALIAS-1
static void set_two(int *a, int *b) { *a = 1; *b = 2; }
int main(void) { int x = 0; set_two(&x, &x); return x - 2; }
