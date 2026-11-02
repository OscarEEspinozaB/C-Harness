// premise: TIP-1 kind: violation
// reason: -Werror=conversion
int main(int argc, char **argv) { (void)argv; char c = argc * 1000; return c == 0; }
