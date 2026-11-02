// premise: EST-1 kind: evasion
// reason: write-to-const
static const int K = 1;
int main(void) { int *p = (int *)(uintptr_t)&K; *p = 2; return K - 1; }
