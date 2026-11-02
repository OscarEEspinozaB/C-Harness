// premisa: TIP tipo: control
union valor { int entero; float real; };
int main(void) { union valor v; v.real = 0.0f; return v.entero; }
