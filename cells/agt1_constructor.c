// premise: AGT-1 kind: violation
// reason: poisoned "__attribute__"
__attribute__((constructor)) static void before(void) { puts("before main"); }
int main(void) { return 0; }
