/* ch_salida.h: un parámetro de salida se marca en la firma y en la llamada (MEM-5). */
#ifndef CH_SALIDA_H
#define CH_SALIDA_H

typedef struct { int *ptr; } ch_sal_int;

#define CH_SAL_INT(variable) ((ch_sal_int){ &(variable) })

#endif
