// premisa: MEM-4 tipo: evasion
// motivo: ESTADO-1
char *modulo_tampon(void);
static char tampon[8];
char *modulo_tampon(void) { return tampon; }
