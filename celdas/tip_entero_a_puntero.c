// premisa: TIP tipo: evasion
int main(void) {
  int x = 0;
  uintptr_t dir = (uintptr_t)&x;
  *(int *)dir = 0;
  return x;
}
