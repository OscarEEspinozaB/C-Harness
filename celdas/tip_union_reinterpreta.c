// premisa: TIP tipo: violacion
union u { int i; float f; };
int main(void) { union u v; v.f = 0.0f; return v.i; }
