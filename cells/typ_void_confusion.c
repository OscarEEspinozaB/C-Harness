// premise: TIP kind: limit
struct a { int x; int y; };
struct b { double v; };
static int read_value(void *p) { struct a *q = p; return q->y == 0 ? 0 : 0; }
int main(void) { struct b v = { 1.0 }; return read_value(&v); }
