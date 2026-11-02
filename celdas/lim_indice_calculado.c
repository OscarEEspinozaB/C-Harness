// premisa: LIM tipo: violacion
// motivo: out of bounds
int main(int argc, char **argv) { (void)argv; int a[4] = {0}; return a[argc + 9]; }
