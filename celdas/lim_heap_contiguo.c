// premisa: LIM tipo: violacion
int main(int argc, char **argv) { (void)argv; int *a = calloc(4, sizeof *a); if (!a) return 1; int r = a[argc + 3]; free(a); return r; }
