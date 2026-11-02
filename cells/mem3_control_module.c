// premise: EST-2 kind: control
int module_next(void);
static int state = 0;
int module_next(void) { return ++state; }
