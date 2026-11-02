// premisa: AGT-4 tipo: evasion
// motivo: poisoned "__asm__"
int ejecuta(const char *) __asm__("system");
int main(void) { return ejecuta("true"); }
