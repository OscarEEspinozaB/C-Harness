// premisa: MEM-5 tipo: control
#include "ch_salida.h"
static void sube(ch_sal_int s) { *s.ptr += 1; }
int main(void) { int x = 0; sube(CH_SAL_INT(x)); return x - 1; }
