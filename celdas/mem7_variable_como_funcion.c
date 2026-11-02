// premisa: MEM-7 tipo: violacion
// motivo: colisión
static int dato(void) { return 0; }
int main(void) { int r = dato(); { int dato = r; r = dato; } return r; }
