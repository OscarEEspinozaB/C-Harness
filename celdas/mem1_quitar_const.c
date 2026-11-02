// premisa: MEM-1 tipo: evasion
static const int K = 1;
int main(void) { int *p = (int *)&K; *p = 2; return K - 1; }
