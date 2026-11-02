// premise: EST-2 kind: control
/* Const data that holds pointers lives in .data.rel.ro, not in mutable state. */
static const char *const names[] = { "a", "b" };
int main(void) { return names[0][1]; }
