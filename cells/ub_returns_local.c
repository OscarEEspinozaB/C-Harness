// premise: UB-1 kind: violation
// reason: return-local-addr
static int *f(void) { int x = 0; return &x; }
int main(void) { return *f(); }
