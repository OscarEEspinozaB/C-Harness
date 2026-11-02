// premise: EFE-1 kind: evasion
// reason: poisoned "__attribute__"
void module_nothing(void);
static void before(void) { puts("before main"); }
void module_nothing(void) {}
__attribute__((section(".init_array"), used)) static void (*const hook)(void) = before;
