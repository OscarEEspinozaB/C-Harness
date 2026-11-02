// premise: MEM-5 kind: control
#include "support/api_ok.h"
int api_read(const int *value) { return *value; }
void api_write(int *value) { *value = 0; }
int main(void) { int x = 0; api_write(&x); return api_read(&x); }
