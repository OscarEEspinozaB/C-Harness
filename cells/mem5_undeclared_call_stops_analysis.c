// premise: INT-1 kind: violation
// reason: implicit declaration
/* The harness must not reason about a function it cannot see: it may write. */
static void fill(char *buffer) { undeclared_writer(buffer); }
int main(void) { char text[2] = { 0, 0 }; fill(text); return text[0]; }
