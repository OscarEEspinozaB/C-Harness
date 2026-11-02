// premise: EFE-3 kind: evasion
// reason: poisoned "__asm__"
int execute(const char *) __asm__("system");
int main(void) { return execute("true"); }
