/* ch_veneno.h: se inyecta con -include. Incluye antes las cabeceras permitidas
 * y luego envenena identificadores: usarlos es un error de compilación.
 * Por BASE-1, cada prohibición declara por qué C no basta para sostenerla. */
#ifndef CH_VENENO_H
#define CH_VENENO_H

#include "ch_cabeceras.h"

/* Escriben sin conocer el tamaño del destino: C ofrece variantes con límite. */
#pragma GCC poison gets strcpy strcat sprintf vsprintf
/* Ejecutan código elegido durante la ejecución: rompen AGT-3 y AGT-4. */
#pragma GCC poison system popen fork execl execv execve execvp dlopen dlsym syscall
/* Reservan en la pila sin comprobación y sin devolver error. */
#pragma GCC poison alloca
/* Saltan entre funciones sin ejecutar la limpieza de quien queda atrás. */
#pragma GCC poison setjmp longjmp
/* No son C estándar y escapan de todo análisis del arnés. */
#pragma GCC poison asm __asm __asm__ __attribute__ __attribute
/* Desactivarían el arnés desde el propio código (ARN-1). */
#pragma GCC poison diagnostic system_header

#endif
