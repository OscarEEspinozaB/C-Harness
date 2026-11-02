// premisa: AGT-3 tipo: violacion
#include <dlfcn.h>
int main(void) { void *h = dlopen("libc.so.6", 2); return h ? 0 : 0; }
