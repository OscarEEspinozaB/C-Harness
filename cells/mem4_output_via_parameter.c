// premise: MEM-4 kind: evasion
// reason: STATE-1
void module_hand_over(int **output);
static int state = 0;
void module_hand_over(int **output) { *output = &state; }
