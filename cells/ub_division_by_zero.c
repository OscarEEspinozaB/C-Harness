// premise: UB kind: violation
// reason: division by zero
int main(int argc, char **argv) { (void)argv; return 1 / (argc - 1); }
