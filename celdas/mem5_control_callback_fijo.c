// premisa: MEM-5 tipo: control
static int lee(int *p) { return *p; }
int main(void) { int (*f)(int *) = lee; int x = 0; return f(&x); }
