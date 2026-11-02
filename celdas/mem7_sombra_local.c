// premisa: MEM-7 tipo: violacion
int main(void) { int x = 0; { int x = 1; (void)x; } return x; }
