// premise: UB-1 kind: violation
// reason: infinite-recursion
static int f(volatile int n) { volatile char b[4096]; b[0] = 0; return f(n + 1) + b[0]; }
int main(void) { return f(0); }
