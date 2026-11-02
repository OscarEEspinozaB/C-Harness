// premise: INT-1 kind: violation
// reason: CONST-1
int read_value(int *p);
int read_value(int *p) { return *p; }
int main(void) { int x = 0; return read_value(&x); }
