// premise: EST-3 kind: control
/* A const table may leave its module: it gives no write access. */
const int *module_table(void);
static const int table[2] = { 1, 2 };
const int *module_table(void) { return table; }
