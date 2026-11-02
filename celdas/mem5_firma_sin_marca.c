// premisa: MEM-5 tipo: violacion
#include "ch_salida.h"
static void sube(int v) { (void)v; }
int main(void) { int x = 0; sube(CH_SAL_INT(x)); return x; }
