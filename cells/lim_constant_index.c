// premise: LIM kind: violation
// reason: buffer over-read
int main(void) { int a[4] = {0}; return a[4]; }
