// premisa: MEM-3 tipo: control
int modulo_siguiente(void);
static int estado = 0;
int modulo_siguiente(void) { return ++estado; }
