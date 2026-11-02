#!/usr/bin/env bash
# FUN-2: C-Harness goes through C-Harness. A mutation proves the pass is not trivial.
set -u
. "$(cd "$(dirname "$0")" && pwd)/harness/stages.sh"
tmp=$(mktemp -d); trap 'rm -rf "$tmp"' EXIT
export ASAN_OPTIONS=detect_leaks=1
build_verifier || { echo "could not build the verifier" >&2; exit 2; }

source_file="$CH_ROOT/verifier/ch_verify.c"
CH_INCLUDES="-isystem $CH_LLVM/include -include clang-c/Index.h"
CH_LINK="-L$CH_LLVM/lib -lclang-19 -Wl,-rpath,$CH_LLVM/lib"
CH_RUN_ARGS="$source_file -I$CH_ROOT/harness -isystem $CH_LLVM/include -include ch_headers.h"
CH_EXTRA_SYMBOLS='^clang_'
CH_CBMC_INCLUDES="-I$CH_LLVM/include -I$CH_ROOT/verifier"
CH_CBMC_ENTRY="$CH_ROOT/verifier/cbmc_entry.c"

measure_harness "$source_file" > "$tmp/stage"; stage=$(cat "$tmp/stage")
if [ -s "$tmp/inconclusive" ]; then
  echo "FUN-2 does not hold: CBMC did not conclude: $(cat "$tmp/inconclusive")"
  exit 1
fi
if [ "$stage" != - ]; then
  echo "FUN-2 does not hold: stage $stage: $(cat "$tmp/reason")"
  exit 1
fi
echo "The verifier passes compile, verifier, symbols, bounded search and sanitized run."

mutated="$tmp/mutated.c"
sed 's/static void report_const(const char \*path, const struct candidate \*c)/static void report_const(const char *path, struct candidate *c)/' "$source_file" > "$mutated"
if cmp -s "$source_file" "$mutated"; then echo "the mutation no longer applies: review it" >&2; exit 2; fi
measure_harness "$mutated" > "$tmp/stage"; stage=$(cat "$tmp/stage")
if [ "$stage" = - ]; then
  echo "FUN-2 does not hold: the mutation passed undetected"
  exit 1
fi
echo "Removing a const from the verifier stops it at stage $stage: $(cat "$tmp/reason")"
