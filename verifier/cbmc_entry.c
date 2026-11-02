/* cbmc_entry.c: entry point for analyzing the verifier with CBMC.
 * CBMC's model leaves the contents of argv undefined; C guarantees that argv[i]
 * is a valid string when i < argc. Here argv has that shape.
 * CBMC does not enter libclang: its functions return unconstrained values. */
#define main ch_program
#include "ch_verify.c"
#undef main

int main(void) {
  char program[] = "ch_verify";
  char file[] = "input.c";
  char *arguments[] = { program, file, NULL };
  return ch_program(2, arguments);
}
