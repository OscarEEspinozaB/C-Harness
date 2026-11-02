// premise: AGT-3 kind: violation
// reason: poisoned "dlopen"
#include <dlfcn.h>
int main(void) { void *h = dlopen("libc.so.6", 2); return h ? 0 : 0; }
