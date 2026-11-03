# zlib v1.3.2 audit

Commit: `da607da739fa6047df13e66a2af6b8bec7c2a498`. Files: 15. Lines: 9703.

## Stage C: poison

In the project:

```
```

In system headers it includes:

```
     90 include/unistd.h
     15 include/x86_64-linux-gnu/sys/types.h
```

## Stage C: warnings without poison

```
     49 -Wcast-qual
     26 -Wswitch-enum
     19 -Wimplicit-fallthrough=
      4 -Wsign-conversion
      2 -Wconversion
      1 -Wcast-align
      1 -Wanalyzer-malloc-leak
```

## Stage V: verifier

```
inftrees.c:46: CONST-1: parameter 'lens' of 'inflate_table' is never written: declare it 'const unsigned short
trees.c:203: CONST-1: parameter 'bl_count' of 'gen_codes' is never written: declare it 'const ushf *'
trees.c:509: CONST-1: parameter 'tree' of 'pqdownheap' is never written: declare it 'const ct_data *'
trees.c:540: CONST-1: parameter 'desc' of 'gen_bitlen' is never written: declare it 'const tree_desc *'
trees.c:753: CONST-1: parameter 'tree' of 'send_tree' is never written: declare it 'const ct_data *'
trees.c:966: CONST-1: parameter 's' of 'detect_data_type' is never written: declare it 'const deflate_state *'
```

## Stage S: symbols

Writable state: 

External symbols outside the list: close, fcntl, lseek, open, read, write

## Stage B: CBMC on a short input of unknown contents

Input of 2 characters: exit status 124 in 100 s. 

Input of 7 characters: exit status 124 in 101 s. 

## Practice counts (exploratory, not a harness stage)

```
functions 159
static_functions 51
lines 6860
longest 680
pointer_params 110
const_pointer_params 34
null_checked_params 10
buffer_params 50
buffer_params_with_size 36
mutable_state 0
const_tables 18
casts 1093
gotos 60
unions 0
function_pointers 0
bounded_calls 50
unbounded_calls 0
direct_alloc 35
```

## Stage E: sanitizers on sample inputs

Inputs: 13. Stopped by a sanitizer: 0.
