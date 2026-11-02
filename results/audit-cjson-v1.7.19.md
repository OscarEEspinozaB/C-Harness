# cJSON v1.7.19 audit

Commit: `c859b25da02955fef659d658b8f324b5cde87be3`. Lines of cJSON.c: 3191.

## Stage C: poison, with cJSON's extra headers loaded first

```
      6 "sprintf"
      5 "strcpy"
      3 "diagnostic"
```

Without preloading, system headers outside the allowed list also hit the poison:

```
```

## Stage C: warnings without poison

```
cJSON.c:611:15: warning: comparing floating-point with ‘==’ or ‘!=’ is unsafe [-Wfloat-equal]
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

## Stage B: CBMC on cJSON_Parse with a short input of unknown contents

Input of 2 characters: exit status 124 in 101 s. 

Input of 4 characters: exit status 124 in 100 s. 

## Stage E: not run (needs --run)
