/* cbmc_entry.c: entry point for analyzing the verifier with CBMC.
 * CBMC's model leaves the contents of argv undefined; C guarantees that argv[i]
 * is a valid string when i < argc. Here argv has that shape.
 * CBMC does not enter libclang: its functions return unconstrained values, and
 * clang_visitChildren never runs the callbacks, so the tree walks stay unanalyzed.
 * clang_getCString is modeled as its contract allows: a valid string or NULL. */
#define main ch_program
#include "ch_verify.c"
#undef main

int ch_nondet_choice(void);

const char *clang_getCString(CXString string) {
  (void)string;
  return ch_nondet_choice() ? "input.c" : NULL;
}

int main(void) {
  char program[] = "ch_verify";
  char file[] = "input.c";
  char *arguments[] = { program, file, NULL };
  return ch_program(2, arguments);
}
