// premisa: MEM-3 tipo: violacion
// motivo: variable exportada
int compartida = 1;
int main(void) { return compartida - 1; }
