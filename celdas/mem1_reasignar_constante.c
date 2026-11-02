// premisa: MEM-1 tipo: violacion
// motivo: read-only variable
static const int K = 1;
int main(void) { K = 2; return K; }
