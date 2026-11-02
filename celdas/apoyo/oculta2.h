# 1 "falsa.h" 3
static inline int oculta(void) { int x = 0; { int x = 1; (void)x; } return x; }
