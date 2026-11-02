/* ch_veneno.h: se inyecta con -include. Incluye antes la cabecera permitida
 * y luego envenena identificadores: usarlos es un error de compilación. */
#ifndef CH_VENENO_H
#define CH_VENENO_H

#include "ch_cabeceras.h"

/* Cadenas sin límite */
#pragma GCC poison gets strcpy strcat sprintf vsprintf
/* Vías de efecto y de ejecución dinámica (AGT-3, AGT-4) */
#pragma GCC poison system popen fork execl execv execve execvp dlopen dlsym syscall
#pragma GCC poison getenv setenv putenv environ
/* Salidas del modelo del lenguaje */
#pragma GCC poison asm __asm __asm__ union goto alloca setjmp longjmp
/* Código que corre antes de main (AGT-1) */
#pragma GCC poison constructor __constructor__ destructor __destructor__
/* Atributos: secciones, alias y otros atajos */
#pragma GCC poison __attribute__ __attribute
/* Silenciar al propio arnés */
#pragma GCC poison diagnostic system_header

#endif
