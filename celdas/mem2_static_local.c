// premisa: MEM-2 tipo: evasion
// motivo: estado en el archivo de main
static int cuenta(void) { static int n = 0; return ++n; }
int main(void) { cuenta(); return cuenta() - 2; }
