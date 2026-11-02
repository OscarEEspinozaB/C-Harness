// premise: MEM-5 kind: limit
static void increment(int *p) { *p += 1; }
int main(void) { int x = 0; int *q = &x; increment(q); return x - 1; }
