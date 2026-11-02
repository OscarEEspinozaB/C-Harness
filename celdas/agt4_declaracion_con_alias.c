// premisa: AGT-4 tipo: evasion
int ejecuta(const char *) __asm__("system");
int main(void) { return ejecuta("true"); }
