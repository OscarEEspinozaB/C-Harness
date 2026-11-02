/* lz4_cbmc_entry.c: CBMC entry point. Decompresses a short input of unknown contents. */
#include "lz4.c"

int main(void) {
  char input[CH_INPUT_SIZE];
  char output[16];
  LZ4_decompress_safe(input, output, CH_INPUT_SIZE, (int)sizeof output);
  return 0;
}
