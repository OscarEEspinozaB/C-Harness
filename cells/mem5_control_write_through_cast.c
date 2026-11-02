// premise: INT-1 kind: control
/* The write goes through a cast: adding const here would discard it. */
static void fill(char *buffer) { unsigned char *bytes = (unsigned char *)buffer; bytes[0] = 0; }
int main(void) { char text[2] = { 1, 0 }; fill(text); return text[0]; }
