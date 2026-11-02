// premise: EST-1 kind: evasion
// reason: cast-qual
static const int K = 1;
int main(void) { int *p = (int *)&K; *p = 2; return K - 1; }
