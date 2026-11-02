// premisa: MEM-5 tipo: violacion
// motivo: CONST-1
static int lee(int *p) { return *p; }
int main(void) { int x = 0; return lee(&x); }
