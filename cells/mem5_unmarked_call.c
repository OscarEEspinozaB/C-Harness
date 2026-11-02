// premise: MEM-5 kind: violation
// reason: makes pointer from integer
static void increment(int *p) { *p += 1; }
int main(void) { int x = 0; increment(x); return x - 1; }
