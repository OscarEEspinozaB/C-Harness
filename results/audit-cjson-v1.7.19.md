# cJSON v1.7.19 audit

Commit: `c859b25da02955fef659d658b8f324b5cde87be3`. Lines of cJSON.c: 3191.

## Stage C: poison

```
      6 "sprintf"
      5 "strcpy"
      3 "diagnostic"
```

## Stage C: warnings without poison

```
      1 -Wfloat-equal
```

## Stage V: verifier

```
cJSON.c:209: CONST-1: parameter 'hooks' of 'cJSON_InitHooks' is never written: declare it 'const cJSON_Hooks *
cJSON.c:1343: CONST-1: parameter 'item' of 'cJSON_PrintPreallocated' is never written: declare it 'const cJSON
cJSON.c:2115: CONST-1: parameter 'item' of 'cJSON_AddItemReferenceToArray' is never written: declare it 'const
cJSON.c:2125: CONST-1: parameter 'item' of 'cJSON_AddItemReferenceToObject' is never written: declare it 'cons
```

## Stage S: symbols

State: b global_error, d global_hooks, b version.1

External symbols outside the list: sprintf, strcpy

## Stage B: CBMC on a short input of unknown contents

Input of 2 characters: exit status 124 in 100 s. 

Input of 4 characters: exit status 124 in 100 s. 

## Practice counts (exploratory, not a harness stage)

```
functions 113
static_functions 34
lines 2861
longest 128
pointer_params 167
const_pointer_params 86
null_checked_params 62
buffer_params 45
buffer_params_with_size 3
mutable_state 3
const_tables 1
casts 528
gotos 43
unions 0
function_pointers 6
bounded_calls 9
unbounded_calls 11
direct_alloc 0
```

## Stage E: not run (needs --run)
