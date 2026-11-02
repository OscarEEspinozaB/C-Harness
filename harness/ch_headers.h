/* ch_headers.h: standard C headers, loaded before the poison.
 * Threads and POSIX stay out: their symbols are not on the allowed list. */
#ifndef CH_HEADERS_H
#define CH_HEADERS_H

#include <ctype.h>
#include <errno.h>
#include <float.h>
#include <limits.h>
#include <locale.h>
#include <math.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#endif
