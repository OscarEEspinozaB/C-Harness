// premise: UB-1 kind: violation
// reason: overflow
int main(int argc, char **argv) { (void)argv; int x = 2147483647; x += argc; return x == 0; }
