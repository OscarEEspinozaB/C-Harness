// premise: MEM-5 kind: control
static int read_value(int *p) { return *p; }
int main(void) { int (*f)(int *) = read_value; int x = 0; return f(&x); }
