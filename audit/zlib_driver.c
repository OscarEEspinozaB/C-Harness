/* zlib_driver.c: compresses one input file and decompresses it again. */
#include "zlib.h"
#include <stdio.h>
#include <string.h>

int main(int argc, char **argv) {
  if (argc != 2) return 2;
  FILE *file = fopen(argv[1], "rb");
  if (!file) return 2;
  static Bytef text[1 << 20];
  static Bytef packed[(1 << 20) + (1 << 12)];
  static Bytef restored[1 << 20];
  uLong length = (uLong)fread(text, 1, sizeof text, file);
  fclose(file);
  uLongf packed_size = sizeof packed;
  if (compress(packed, &packed_size, text, length) != Z_OK) return 1;
  uLongf restored_size = sizeof restored;
  if (uncompress(restored, &restored_size, packed, packed_size) != Z_OK) return 1;
  return restored_size == length && memcmp(text, restored, length) == 0 ? 0 : 1;
}
