// premise: INT-1 kind: control
static void increment(int *p) { *p += 1; }
static int read_value(const int *p) { return *p; }
int main(void) { int x = 0; increment(&x); return read_value(&x) - 1; }
