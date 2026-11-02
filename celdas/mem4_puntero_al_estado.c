// premisa: MEM-4 tipo: evasion
// motivo: estado
int *modulo_estado(void);
static int estado = 0;
int *modulo_estado(void) { return &estado; }
