# Harness stages. Sourced by run.sh and self-check.sh.
# Bash only coordinates: GCC, nm, CBMC, the sanitizers and ch_verify decide.
CH_ROOT=$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)
. "$CH_ROOT/harness/flags.sh"
CH_LLVM=/usr/lib/llvm-19
CH_VERIFIER="$CH_ROOT/bin/ch_verify"

build_verifier() {
  mkdir -p "$CH_ROOT/bin"
  gcc -std=c17 -O2 -isystem "$CH_LLVM/include" -I"$CH_ROOT/harness" -include ch_headers.h \
    "$CH_ROOT/verifier/ch_verify.c" -o "$CH_VERIFIER" \
    -L"$CH_LLVM/lib" -lclang-19 -Wl,-rpath,"$CH_LLVM/lib"
}

# Returns 1 and writes the reason to $tmp/reason if the object breaks a symbol rule.
check_symbols() {
  local table external
  table=$(nm "$1")
  if grep -qE ' [DBCG] ' <<<"$table"; then
    grep -m1 -E ' [DBCG] ' <<<"$table" | cut -d' ' -f2- | sed 's/^/exported variable: /' > "$tmp/reason"; return 1
  fi
  if grep -qE ' T main$' <<<"$table" && grep -qE ' [dbs] ' <<<"$table"; then
    grep -m1 -E ' [dbs] ' <<<"$table" | cut -d' ' -f2- | sed 's/^/state in the main file: /' > "$tmp/reason"; return 1
  fi
  external=$(awk '$1 == "U" {print $2}' <<<"$table" | grep -vxFf "$CH_ROOT/harness/allowed_symbols.txt" | grep -vE "${CH_EXTRA_SYMBOLS:-^$}" | head -1)
  if [ -n "$external" ]; then
    echo "symbol not allowed: $external" > "$tmp/reason"; return 1
  fi
  return 0
}

# Bounded search with CBMC. Returns 1 only if CBMC finds a failure.
# If it does not conclude (timeout, unsupported construct), it records that in $tmp/inconclusive.
bounded_search() {
  local wrapper="$tmp/bounded.c" status
  printf '#include "ch_headers.h"\n#include "%s"\n' "${CH_CBMC_ENTRY:-$1}" > "$wrapper"
  # shellcheck disable=SC2086
  timeout "${CH_CBMC_TIMEOUT:-120}" cbmc -I"$CH_ROOT/harness" -I"$(dirname "$1")" ${CH_CBMC_INCLUDES:-} --bounds-check --pointer-check --div-by-zero-check \
    --unwind "${CH_CBMC_UNWIND:-5}" --no-unwinding-assertions "$wrapper" >"$tmp/b.log" 2>&1
  status=$?
  [ $status -eq 0 ] && return 0
  if [ $status -ne 10 ]; then echo "$(basename "$1"): exit status $status" >> "$tmp/inconclusive"; return 0; fi
  grep ': FAILURE$' "$tmp/b.log" | sed -E 's/^\[[^]]*\] //; s/: FAILURE$//; s/\|/\\|/g' | paste -sd';' > "$tmp/reason"
  return 1
}

# Prints the stage that stopped the file (C, V, S, B, E) or "-". Reason in $tmp/reason.
# Extra options: CH_INCLUDES (compile), CH_LINK and CH_RUN_ARGS.
measure_harness() {
  : > "$tmp/reason"
  # shellcheck disable=SC2086
  if ! gcc $CH_STATIC_FLAGS -I"$CH_ROOT/harness" -I"$(dirname "$1")" ${CH_INCLUDES:-} -include ch_poison.h -c "$1" -o "$tmp/a.o" 2>"$tmp/c.log"; then
    grep -m1 'error' "$tmp/c.log" | sed 's/^[^ ]* error: //; s/|/\\|/g' | cut -c1-90 > "$tmp/reason"; echo C; return
  fi
  # shellcheck disable=SC2086
  "$CH_VERIFIER" "$1" -I"$CH_ROOT/harness" -I"$(dirname "$1")" ${CH_INCLUDES:-} -include ch_headers.h >"$tmp/v.log" 2>"$tmp/v.err"
  local status=$?
  if [ $status -ne 0 ]; then
    if [ $status -eq 1 ]; then sed -n '1s/^[^ ]* //p' "$tmp/v.log" | cut -c1-90 > "$tmp/reason"
    else head -1 "$tmp/v.err" | cut -c1-90 > "$tmp/reason"; fi
    echo V; return
  fi
  # shellcheck disable=SC2086
  gcc -std=c17 -O0 -I"$CH_ROOT/harness" -I"$(dirname "$1")" ${CH_INCLUDES:-} -include ch_poison.h -c "$1" -o "$tmp/s.o" 2>/dev/null
  check_symbols "$tmp/s.o" || { echo S; return; }
  nm "$tmp/s.o" | grep -q ' T main$' || { echo -; return; }
  bounded_search "$1" || { echo B; return; }
  # shellcheck disable=SC2086
  if ! gcc $CH_DYNAMIC_FLAGS -I"$CH_ROOT/harness" -I"$(dirname "$1")" ${CH_INCLUDES:-} -include ch_poison.h "$1" -o "$tmp/e" ${CH_LINK:-} 2>"$tmp/c.log"; then
    grep -m1 'error' "$tmp/c.log" | cut -c1-90 > "$tmp/reason"; echo C; return
  fi
  # shellcheck disable=SC2086
  timeout 120 "$tmp/e" ${CH_RUN_ARGS:-} >/dev/null 2>"$tmp/e.log"; status=$?
  [ $status -eq 0 ] && { echo -; return; }
  grep -m1 -E 'ERROR: |runtime error' "$tmp/e.log" | sed -E 's/.*(ERROR: [A-Za-z]+: |runtime error: )//' | cut -c1-60 > "$tmp/reason"
  [ -s "$tmp/reason" ] || echo "exit status $status" > "$tmp/reason"
  echo E
}
