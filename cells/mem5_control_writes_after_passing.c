// premise: INT-1 kind: control
static void set_zero(int *p) { *p = 0; }
static void forward(int *p) { set_zero(p); }
int main(void) { int x = 1; forward(&x); return x; }
