// premisa: MEM-5 tipo: control
static void sube(int *p) { *p += 1; }
static int lee(const int *p) { return *p; }
int main(void) { int x = 0; sube(&x); return lee(&x) - 1; }
