// premise: UB-1 kind: violation
// reason: -Werror=uninitialized
int main(void) { int x; return x; }
