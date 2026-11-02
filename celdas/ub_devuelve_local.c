// premisa: UB tipo: violacion
static int *f(void) { int x = 0; return &x; }
int main(void) { return *f(); }
