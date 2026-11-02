// premisa: MEM-4 tipo: control
const int *modulo_lee(void);
int modulo_sube(void);
static int estado = 0;
const int *modulo_lee(void) { return &estado; }
int modulo_sube(void) { int *p = &estado; *p += 1; return *p; }
