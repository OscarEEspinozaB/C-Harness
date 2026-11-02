// premise: INT-1 kind: violation
// reason: makes integer from pointer
static int increment(int v) { return v + 1; }
int main(void) { int x = 0; return increment(&x) - 1; }
