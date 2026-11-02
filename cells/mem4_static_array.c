// premise: EST-3 kind: evasion
// reason: STATE-1
char *module_buffer(void);
static char buffer[8];
char *module_buffer(void) { return buffer; }
