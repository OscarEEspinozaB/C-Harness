// premisa: MEM-5 tipo: control
static void pone(int *p) { *p = 0; }
static void reenvia(int *p) { pone(p); }
int main(void) { int x = 1; reenvia(&x); return x; }
