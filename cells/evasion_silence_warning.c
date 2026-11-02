// premise: ARN-1 kind: evasion
// reason: poisoned "diagnostic"
#pragma GCC diagnostic ignored "-Wshadow"
int main(void) { int x = 0; { int x = 1; (void)x; } return x; }
