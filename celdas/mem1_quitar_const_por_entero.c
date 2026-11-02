// premisa: MEM-1 tipo: evasion
// motivo: write-to-const
static const int K = 1;
int main(void) { int *p = (int *)(uintptr_t)&K; *p = 2; return K - 1; }
