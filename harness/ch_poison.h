/* ch_poison.h: injected with -include. Loads the allowed headers first, then
 * poisons identifiers: using one is a compile error.
 * Under FUN-1, each ban states why C alone cannot uphold it. */
#ifndef CH_POISON_H
#define CH_POISON_H

#include "ch_headers.h"

/* They write without knowing the destination size: C offers bounded variants. */
#pragma GCC poison gets strcpy strcat sprintf vsprintf
/* They run code chosen at run time: they break EFE-2 and EFE-3. */
#pragma GCC poison system popen fork execl execv execve execvp dlopen dlsym syscall
/* It reserves stack space with no check and no error result. */
#pragma GCC poison alloca
/* They jump between functions without running the cleanup left behind. */
#pragma GCC poison setjmp longjmp
/* Not standard C, and they escape every analysis in the harness. */
#pragma GCC poison asm __asm __asm__ __attribute__ __attribute
/* They would turn the harness off from inside the code (ARN-1). */
#pragma GCC poison diagnostic system_header

#endif
