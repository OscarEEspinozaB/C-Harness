// premisa: MEM-5 tipo: violacion
// motivo: CONST-1
int lee(int *p);
int lee(int *p) { return *p; }
int main(void) { int x = 0; return lee(&x); }
