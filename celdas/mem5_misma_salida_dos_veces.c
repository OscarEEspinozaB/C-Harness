// premisa: MEM-5 tipo: violacion
#include "ch_salida.h"
static void dos(ch_sal_int a, ch_sal_int b) { *a.ptr = 1; *b.ptr = 2; }
int main(void) { int x = 0; dos(CH_SAL_INT(x), CH_SAL_INT(x)); return x - 2; }
