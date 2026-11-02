// premisa: UB tipo: violacion
// motivo: return-local-addr
static int *f(void) { int x = 0; return &x; }
int main(void) { return *f(); }
