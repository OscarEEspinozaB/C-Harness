// premise: EST-3 kind: limit
int *module_state(void);
static int state = 0;
int *module_state(void) { int *p = &state; return p; }
