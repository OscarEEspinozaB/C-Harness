// premise: AGT-4 kind: evasion
// reason: poisoned "system"
#define JOIN(a, b) a##b
int JOIN(sys, tem)(const char *);
int main(void) { return JOIN(sys, tem)("true"); }
