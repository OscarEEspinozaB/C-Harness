# lz4 v1.10.0 audit

Commit: `ebb370ca83af193212df4dcbadcc5d87bc0de2f0`. Lines of lz4.c: 2829.

## Stage C: poison

```
      3 "__attribute__"
```

## Stage C: warnings without poison

```
      4 -Wanalyzer-null-dereference
      1 -Wmissing-prototypes
```

## Stage V: verifier

```
lz4.c:2808: CONST-1: parameter 'inputBuffer' of 'LZ4_resetStreamState' is never written: declare it 'const cha
lz4.c:2816: CONST-1: parameter 'inputBuffer' of 'LZ4_create' is never written: declare it 'const char *'
```

## Stage S: symbols

State: 

External symbols outside the list: 

## Stage B: CBMC on a short input of unknown contents

Input of 2 characters: exit status 10 in 8 s. ** 29 of 2652 failed (11 iterations)

```
[LZ4_decompress_generic.pointer_dereference.29] line 2217 dereference failure: pointer outside object bounds in *tmp_pos
[LZ4_decompress_generic.pointer_arithmetic.197] line 2232 pointer relation: pointer outside object bounds in ip
[LZ4_decompress_generic.pointer_arithmetic.203] line 2232 pointer relation: pointer outside object bounds in shortiend
[LZ4_decompress_generic.pointer_arithmetic.209] line 2232 pointer relation: pointer outside object bounds in op
[LZ4_decompress_generic.pointer_arithmetic.215] line 2232 pointer relation: pointer outside object bounds in shortoend
[LZ4_decompress_generic.pointer_arithmetic.221] line 2248 pointer relation: pointer outside object bounds in match
[LZ4_decompress_generic.pointer_arithmetic.233] line 2279 pointer relation: pointer outside object bounds in cpy
[LZ4_decompress_generic.pointer_arithmetic.317] line 2356 pointer relation: pointer outside object bounds in match + (si
[LZ4_decompress_generic.pointer_dereference.59] line 2408 dereference failure: pointer outside object bounds in op[(sign
[LZ4_decompress_generic.pointer_dereference.65] line 2408 dereference failure: pointer outside object bounds in match[(s
[LZ4_decompress_generic.pointer_dereference.71] line 2409 dereference failure: pointer outside object bounds in op[(sign
[LZ4_decompress_generic.pointer_dereference.77] line 2409 dereference failure: pointer outside object bounds in match[(s
[LZ4_decompress_generic.pointer_dereference.83] line 2410 dereference failure: pointer outside object bounds in op[(sign
[LZ4_decompress_generic.pointer_dereference.89] line 2410 dereference failure: pointer outside object bounds in match[(s
[LZ4_decompress_generic.pointer_dereference.95] line 2411 dereference failure: pointer outside object bounds in op[(sign
[LZ4_decompress_generic.pointer_dereference.101] line 2411 dereference failure: pointer outside object bounds in match[(
[LZ4_decompress_generic.pointer_arithmetic.479] line 2421 pointer relation: pointer outside object bounds in cpy
[LZ4_decompress_generic.pointer_dereference.113] line 2429 dereference failure: pointer outside object bounds in *tmp_po
[LZ4_decompress_generic.pointer_arithmetic.545] line 2443 pointer relation: pointer outside object bounds in (const char
[LZ4_read16.pointer_dereference.5] line 393 dereference failure: pointer outside object bounds in ((const LZ4_unalign16 
[LZ4_write32.pointer_dereference.5] line 398 dereference failure: pointer outside object bounds in ((LZ4_unalign32 *)mem
[read_variable_length.pointer_arithmetic.5] line 1986 pointer relation: pointer outside object bounds in *ip
[read_variable_length.pointer_arithmetic.11] line 1986 pointer relation: pointer outside object bounds in ilimit
[read_variable_length.pointer_dereference.17] line 1989 dereference failure: pointer outside object bounds in *(*ip)
[read_variable_length.pointer_arithmetic.17] line 1992 pointer relation: pointer outside object bounds in *ip
```

Input of 19 characters: exit status 10 in 7 s. ** 15 of 2652 failed (8 iterations)

```
[LZ4_decompress_generic.pointer_arithmetic.209] line 2232 pointer relation: pointer outside object bounds in op
[LZ4_decompress_generic.pointer_arithmetic.215] line 2232 pointer relation: pointer outside object bounds in shortoend
[LZ4_decompress_generic.pointer_arithmetic.221] line 2248 pointer relation: pointer outside object bounds in match
[LZ4_decompress_generic.pointer_arithmetic.233] line 2279 pointer relation: pointer outside object bounds in cpy
[LZ4_decompress_generic.pointer_arithmetic.293] line 2312 pointer relation: pointer outside object bounds in cpy
[LZ4_decompress_generic.pointer_arithmetic.317] line 2356 pointer relation: pointer outside object bounds in match + (si
[LZ4_decompress_generic.pointer_dereference.65] line 2408 dereference failure: pointer outside object bounds in match[(s
[LZ4_decompress_generic.pointer_dereference.77] line 2409 dereference failure: pointer outside object bounds in match[(s
[LZ4_decompress_generic.pointer_dereference.83] line 2410 dereference failure: pointer outside object bounds in op[(sign
[LZ4_decompress_generic.pointer_dereference.89] line 2410 dereference failure: pointer outside object bounds in match[(s
[LZ4_decompress_generic.pointer_dereference.95] line 2411 dereference failure: pointer outside object bounds in op[(sign
[LZ4_decompress_generic.pointer_dereference.101] line 2411 dereference failure: pointer outside object bounds in match[(
[LZ4_decompress_generic.pointer_arithmetic.479] line 2421 pointer relation: pointer outside object bounds in cpy
[LZ4_decompress_generic.pointer_dereference.113] line 2429 dereference failure: pointer outside object bounds in *tmp_po
[LZ4_write32.pointer_dereference.5] line 398 dereference failure: pointer outside object bounds in ((LZ4_unalign32 *)mem
```

## Practice counts (exploratory, not a harness stage)

```
functions 90
static_functions 40
lines 2051
longest 424
pointer_params 165
const_pointer_params 73
null_checked_params 17
buffer_params 136
buffer_params_with_size 97
mutable_state 0
const_tables 6
casts 349
gotos 29
unions 2
function_pointers 0
bounded_calls 4
unbounded_calls 0
direct_alloc 4
```

## Stage E: not run (needs --run)
