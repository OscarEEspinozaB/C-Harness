/* zlib_cbmc_entry.c: CBMC entry point. Decompresses a short input of unknown contents. */
#include "zlib.h"

int main(void) {
  Bytef input[CH_INPUT_SIZE];
  Bytef output[16];
  uLongf output_size = sizeof output;
  uncompress(output, &output_size, input, CH_INPUT_SIZE);
  return 0;
}
