// premise: INT-1 kind: control
static void set_two(int *a, int *b) { *a = 1; *b = 2; }
int main(void) { int x = 0; int y = 0; set_two(&x, &y); return x + y - 3; }
