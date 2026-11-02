// premise: MEM-8 kind: control
struct node { struct node *next; int v; };
int main(void) {
  struct node *head = NULL;
  for (int i = 0; i < 4; i++) {
    struct node *n = malloc(sizeof *n);
    if (!n) break;
    n->next = head;
    n->v = i;
    head = n;
  }
  while (head) { struct node *s = head->next; free(head); head = s; }
  return 0;
}
