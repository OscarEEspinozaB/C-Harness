# 1 "fake.h" 3
static inline int hidden(void) { int x = 0; { int x = 1; (void)x; } return x; }
