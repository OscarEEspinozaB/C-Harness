// premise: MEM-6 kind: violation
// reason: nested functions
int main(void) {
  int x = 0;
  void increment(void) { x++; }
  increment();
  return x - 1;
}
