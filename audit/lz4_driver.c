/* lz4_driver.c: compresses one input file and decompresses it again. */
#include "lz4.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv) {
  if (argc != 2) return 2;
  FILE *file = fopen(argv[1], "rb");
  if (!file) return 2;
  static char text[1 << 20];
  static char packed[LZ4_COMPRESSBOUND(1 << 20)];
  static char restored[1 << 20];
  int length = (int)fread(text, 1, sizeof text, file);
  fclose(file);
  int packed_size = LZ4_compress_default(text, packed, length, (int)sizeof packed);
  if (length > 0 && packed_size <= 0) return 1;
  int restored_size = LZ4_decompress_safe(packed, restored, packed_size, (int)sizeof restored);
  return restored_size == length && memcmp(text, restored, (size_t)length) == 0 ? 0 : 1;
}
