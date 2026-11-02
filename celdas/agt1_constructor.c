// premisa: AGT-1 tipo: violacion
__attribute__((constructor)) static void antes(void) { puts("antes de main"); }
int main(void) { return 0; }
