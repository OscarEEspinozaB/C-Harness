// premisa: MEM-5 tipo: violacion
// motivo: makes integer from pointer
static int sube(int v) { return v + 1; }
int main(void) { int x = 0; return sube(&x) - 1; }
