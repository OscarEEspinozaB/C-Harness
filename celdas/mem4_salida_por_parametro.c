// premisa: MEM-4 tipo: evasion
// motivo: ESTADO-1
void modulo_entrega(int **salida);
static int estado = 0;
void modulo_entrega(int **salida) { *salida = &estado; }
