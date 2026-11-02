// premise: MEM-7 kind: violation
// reason: NAME-1
static int datum(void) { return 0; }
int main(void) { int r = datum(); { int datum = r; r = datum; } return r; }
