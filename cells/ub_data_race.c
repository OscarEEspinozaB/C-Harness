// premise: UB-1 kind: evasion
// reason: not allowed: pthread_create
typedef unsigned long thread_id;
int pthread_create(thread_id *, const void *, void *(*)(void *), void *);
int pthread_join(thread_id, void **);
static void *worker(void *p) { int *v = p; for (int i = 0; i < 100000; i++) (*v)++; return NULL; }
int main(void) {
  int *v = calloc(1, sizeof *v);
  if (!v) return 1;
  thread_id a, b;
  pthread_create(&a, NULL, worker, v);
  pthread_create(&b, NULL, worker, v);
  pthread_join(a, NULL);
  pthread_join(b, NULL);
  free(v);
  return 0;
}
