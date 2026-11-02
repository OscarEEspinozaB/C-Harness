// premise: EFE-3 kind: violation
// reason: poisoned "system"
int main(void) { return system("true"); }
