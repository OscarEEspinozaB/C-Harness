// premisa: MEM-5 tipo: violacion
#include "ch_salida.h"
static void sube(ch_sal_int s) { *s.ptr += 1; }
int main(void) { int x = 0; sube(&x); return x - 1; }
