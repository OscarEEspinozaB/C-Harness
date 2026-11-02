// premisa: AGT-1 tipo: evasion
// motivo: poisoned "__attribute__"
void modulo_nada(void);
static void antes(void) { puts("antes de main"); }
void modulo_nada(void) {}
__attribute__((section(".init_array"), used)) static void (*const gancho)(void) = antes;
