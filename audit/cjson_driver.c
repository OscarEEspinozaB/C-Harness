/* cjson_driver.c: runs cJSON on one input file. Invalid JSON is a normal outcome. */
#include "cJSON.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {
  if (argc != 2) return 2;
  FILE *file = fopen(argv[1], "rb");
  if (!file) return 2;
  static char text[1 << 20];
  size_t length = fread(text, 1, sizeof text - 1, file);
  fclose(file);
  text[length] = '\0';
  cJSON *tree = cJSON_Parse(text);
  if (tree) {
    char *printed = cJSON_Print(tree);
    cJSON_free(printed);
    cJSON_Delete(tree);
  }
  return 0;
}
