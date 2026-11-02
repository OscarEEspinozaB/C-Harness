// premise: EST-2 kind: violation
// reason: exported variable
int shared = 1;
int main(void) { return shared - 1; }
