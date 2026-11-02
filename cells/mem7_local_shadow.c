// premise: INT-3 kind: violation
// reason: shadow
int main(void) { int x = 0; { int x = 1; (void)x; } return x; }
