// premise: UB kind: violation
// reason: overflow
/* Overflows with a realistic argc: the argc bound must not hide it. */
int main(int argc, char **argv) { (void)argv; int n = argc * 3000; return n > 0 ? 0 : 1; }
