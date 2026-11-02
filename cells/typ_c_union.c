// premise: TIP kind: control
union value { int integer; float real; };
int main(void) { union value v; v.real = 0.0f; return v.integer; }
