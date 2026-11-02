// premisa: UB tipo: control
/* argc + 1 no puede desbordar en un sistema real. */
int main(int argc, char **argv) { (void)argv; int n = argc + 1; return n > 0 ? 0 : 1; }
