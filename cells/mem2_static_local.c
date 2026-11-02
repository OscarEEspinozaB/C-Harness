// premise: EST-2 kind: evasion
// reason: state in the main file
static int counter(void) { static int n = 0; return ++n; }
int main(void) { counter(); return counter() - 2; }
