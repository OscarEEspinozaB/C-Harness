// premisa: AGT-1 tipo: evasion
void modulo_nada(void);
static void antes(void) { puts("antes de main"); }
void modulo_nada(void) {}
__attribute__((section(".init_array"), used)) static void (*const gancho)(void) = antes;
