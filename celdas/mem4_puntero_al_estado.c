// premisa: MEM-4 tipo: evasion
// motivo: ESTADO-1
int *modulo_estado(void);
static int estado = 0;
int *modulo_estado(void) { return &estado; }
