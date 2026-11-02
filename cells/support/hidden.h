#pragma GCC system_header
static inline int hidden(void) { int x = 0; { int x = 1; (void)x; } return x; }
