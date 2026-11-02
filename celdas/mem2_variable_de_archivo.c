// premisa: MEM-2 tipo: violacion
// motivo: estado en el archivo de main
static int total = 0;
static void suma(void) { total++; }
int main(void) { suma(); return total - 1; }
