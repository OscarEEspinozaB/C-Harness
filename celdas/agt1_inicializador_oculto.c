// premisa: AGT-1 tipo: evasion
// motivo: poisoned "__attribute__"
static void antes(void) { puts("antes de main"); }
__attribute__((section(".init_array"), used)) static void (*const gancho)(void) = antes;
int main(void) { return 0; }
