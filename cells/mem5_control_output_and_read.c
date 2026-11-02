// premise: INT-1 kind: control
static void copy(int *destination, const int *source) { *destination = *source; }
int main(void) { int x = 0; copy(&x, &x); return x; }
