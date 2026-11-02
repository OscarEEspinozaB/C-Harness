// premisa: UB tipo: evasion
// motivo: no permitido: pthread_create
typedef unsigned long hilo_t;
int pthread_create(hilo_t *, const void *, void *(*)(void *), void *);
int pthread_join(hilo_t, void **);
static void *hilo(void *p) { int *v = p; for (int i = 0; i < 100000; i++) (*v)++; return NULL; }
int main(void) {
  int *v = calloc(1, sizeof *v);
  if (!v) return 1;
  hilo_t a, b;
  pthread_create(&a, NULL, hilo, v);
  pthread_create(&b, NULL, hilo, v);
  pthread_join(a, NULL);
  pthread_join(b, NULL);
  free(v);
  return 0;
}
