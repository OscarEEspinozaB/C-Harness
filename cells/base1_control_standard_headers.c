// premise: BASE-1 kind: control
/* Standard C headers must not trip the poison. */
#include <ctype.h>
#include <float.h>
#include <limits.h>
#include <locale.h>
#include <math.h>
#include <stdarg.h>
int main(void) { return tolower('A') == 'a' && INT_MAX > 0 && DBL_MAX > 0.0 && localeconv() != NULL ? 0 : 1; }
