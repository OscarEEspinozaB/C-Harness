// premise: AGT-1 kind: evasion
// reason: poisoned "__attribute__"
static void before(void) { puts("before main"); }
__attribute__((section(".init_array"), used)) static void (*const hook)(void) = before;
int main(void) { return 0; }
