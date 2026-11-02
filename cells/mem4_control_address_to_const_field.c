// premise: EST-3 kind: control
/* Storing a static address in a pointer to const gives no write access. */
struct view { const int *values; };
void module_fill(struct view *out);
static int counter = 0;
void module_fill(struct view *out) { out->values = &counter; }
