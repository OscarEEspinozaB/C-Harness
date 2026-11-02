// premise: INT-1 kind: violation
// reason: 'value' of 'api_read'
#include "support/api.h"
int api_read(int *value) { return *value; }
void api_write(int *value) { *value = 0; }
int main(void) { int x = 0; api_write(&x); return api_read(&x); }
