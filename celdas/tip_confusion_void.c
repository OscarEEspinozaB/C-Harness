// premisa: TIP tipo: limite
struct a { int x; int y; };
struct b { double v; };
static int lee(void *p) { struct a *q = p; return q->y == 0 ? 0 : 0; }
int main(void) { struct b v = { 1.0 }; return lee(&v); }
