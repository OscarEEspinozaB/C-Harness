// premise: MEM-2 kind: violation
// reason: state in the main file
static int total = 0;
static void add(void) { total++; }
int main(void) { add(); return total - 1; }
