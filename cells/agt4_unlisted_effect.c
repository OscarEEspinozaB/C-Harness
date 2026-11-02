// premise: AGT-4 kind: evasion
// reason: not allowed: unlink
int unlink(const char *path);
int main(void) { return unlink("/nonexistent/c-harness") == 0; }
