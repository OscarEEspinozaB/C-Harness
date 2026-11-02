// premisa: MEM-4 tipo: limite
int *modulo_estado(void);
static int estado = 0;
int *modulo_estado(void) { int *p = &estado; return p; }
