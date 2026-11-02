// premisa: MEM-6 tipo: violacion
// motivo: nested functions
int main(void) {
  int x = 0;
  void sube(void) { x++; }
  sube();
  return x - 1;
}
