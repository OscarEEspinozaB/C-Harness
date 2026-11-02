// premise: INT-2 kind: violation
// reason: aliases
static void set_two(int *restrict a, int *restrict b) { *a = 1; *b = 2; }
int main(void) { int x = 0; set_two(&x, &x); return x - 2; }
