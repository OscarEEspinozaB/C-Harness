// premisa: MEM-7 tipo: violacion
// motivo: NOMBRE-1
static int dato(void) { return 0; }
int main(void) { int r = dato(); { int dato = r; r = dato; } return r; }
