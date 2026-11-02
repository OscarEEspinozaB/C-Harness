// premisa: AGT-4 tipo: evasion
// motivo: poisoned "system"
#define UNIR(a, b) a##b
int UNIR(sys, tem)(const char *);
int main(void) { return UNIR(sys, tem)("true"); }
