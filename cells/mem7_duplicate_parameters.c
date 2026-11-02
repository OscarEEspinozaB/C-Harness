// premise: MEM-7 kind: violation
// reason: redefinition of parameter
static int f(int a, int a) { return a; }
int main(void) { return f(0, 0); }
