// premise: MEM-1 kind: violation
// reason: read-only variable
static const int K = 1;
int main(void) { K = 2; return K; }
