// premise: EST-3 kind: evasion
// reason: STATE-1
int *module_state(void);
static int state = 0;
int *module_state(void) { return &state; }
