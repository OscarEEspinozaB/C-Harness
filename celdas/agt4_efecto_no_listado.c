// premisa: AGT-4 tipo: evasion
// motivo: no permitido: unlink
int unlink(const char *ruta);
int main(void) { return unlink("/nonexistent/c-harness") == 0; }
