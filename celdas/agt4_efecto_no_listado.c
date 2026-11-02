// premisa: AGT-4 tipo: evasion
int unlink(const char *ruta);
int main(void) { return unlink("/nonexistent/c-harness") == 0; }
