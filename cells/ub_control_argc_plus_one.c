// premise: UB kind: control
/* argc + 1 cannot overflow on a real system. */
int main(int argc, char **argv) { (void)argv; int n = argc + 1; return n > 0 ? 0 : 1; }
