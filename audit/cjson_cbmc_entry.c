/* cjson_cbmc_entry.c: CBMC entry point. Parses a short string of unknown contents. */
#include "cJSON.c"

int main(void) {
  char text[CH_INPUT_SIZE];
  text[CH_INPUT_SIZE - 1] = '\0';
  cJSON *tree = cJSON_Parse(text);
  if (tree) cJSON_Delete(tree);
  return 0;
}
