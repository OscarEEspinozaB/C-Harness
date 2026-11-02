// premisa: MEM-5 tipo: control
static void copia(int *destino, const int *origen) { *destino = *origen; }
int main(void) { int x = 0; copia(&x, &x); return x; }
