// premise: UB kind: violation
// reason: -Werror=uninitialized
int main(void) { int x; return x; }
