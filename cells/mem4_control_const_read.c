// premise: EST-3 kind: control
const int *module_read(void);
int module_increment(void);
static int state = 0;
const int *module_read(void) { return &state; }
int module_increment(void) { int *p = &state; *p += 1; return *p; }
