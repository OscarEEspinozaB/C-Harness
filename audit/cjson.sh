#!/usr/bin/env bash
# Audits an unmodified cJSON, one stage at a time, without stopping at the first finding.
# cJSON is downloaded, never vendored: it has its own license (MIT).
# By default nothing from cJSON is executed. Pass --run to add the sanitizer stage,
# which compiles cJSON and runs it on the inputs it ships with.
set -u
. "$(cd "$(dirname "$0")" && pwd)/../harness/stages.sh"
tmp=$(mktemp -d); trap 'rm -rf "$tmp"' EXIT
export ASAN_OPTIONS=detect_leaks=1
run_stage=0
[ "${1:-}" = "--run" ] && run_stage=1
build_verifier || exit 2

TAG=v1.7.19
COMMIT=c859b25da02955fef659d658b8f324b5cde87be3
work="$CH_ROOT/bin/audit"
src="$work/cJSON"
[ -d "$src" ] || git clone -q --depth 1 --branch "$TAG" https://github.com/DaveGamble/cJSON "$src" || exit 2
if [ "$(git -C "$src" rev-parse HEAD)" != "$COMMIT" ]; then echo "unexpected cJSON commit" >&2; exit 2; fi
target="$src/cJSON.c"
extra="-include limits.h -include math.h -include ctype.h -include float.h -include locale.h -include stdarg.h"

echo "# cJSON $TAG audit"
echo
echo "Commit: \`$COMMIT\`. Lines of cJSON.c: $(wc -l < "$target")."

echo; echo "## Stage C: poison, with cJSON's extra headers loaded first"
echo; echo '```'
# shellcheck disable=SC2086
gcc $CH_STATIC_FLAGS -I"$CH_ROOT/harness" -I"$src" $extra -include ch_poison.h -fmax-errors=0 -c "$target" -o "$tmp/o.o" 2>&1 \
  | grep -E 'error: attempt to use poisoned' | sed -E 's/.*poisoned //' | sort | uniq -c | sort -rn
echo '```'
echo; echo "Without preloading, system headers outside the allowed list also hit the poison:"
echo; echo '```'
# shellcheck disable=SC2086
gcc $CH_STATIC_FLAGS -I"$CH_ROOT/harness" -I"$src" -include ch_poison.h -fmax-errors=0 -c "$target" -o "$tmp/o.o" 2>&1 \
  | grep -E 'error: attempt to use poisoned' | sed -E 's/:[0-9]+:[0-9]+: error.*//' | grep -v 'cJSON' | sort | uniq -c | sort -rn | sed "s#/usr/##"
echo '```'

echo; echo "## Stage C: warnings without poison"
echo; echo '```'
# shellcheck disable=SC2086
gcc ${CH_STATIC_FLAGS/-Werror/} -Wno-error -I"$src" -c "$target" -o "$tmp/o.o" 2>&1 | grep 'warning:' | sed -E 's#^[^:]*/##' | cut -c1-110
echo '```'

echo; echo "## Stage V: verifier"
echo; echo '```'
"$CH_VERIFIER" "$target" -I"$src" -I"$CH_ROOT/harness" -include ch_headers.h 2>&1 | sed -E 's#^[^:]*/##' | cut -c1-110
echo '```'

echo; echo "## Stage S: symbols"
gcc -std=c17 -O0 -I"$src" -c "$target" -o "$tmp/s.o"
echo; echo "State: $(nm "$tmp/s.o" | grep -E ' [DBCGdbs] ' | cut -d' ' -f2- | paste -sd, | sed 's/,/, /g')"
echo; echo "External symbols outside the list: $(nm "$tmp/s.o" | awk '$1=="U"{print $2}' | grep -vxFf "$CH_ROOT/harness/allowed_symbols.txt" | paste -sd, | sed 's/,/, /g')"

echo; echo "## Stage B: CBMC on cJSON_Parse with a short input of unknown contents"
for size in 3 5; do
  start=$SECONDS
  # shellcheck disable=SC2086
  timeout 100 cbmc -I"$src" -DCH_INPUT_SIZE=$size --bounds-check --pointer-check --div-by-zero-check --unwind 8 --no-unwinding-assertions "$CH_ROOT/audit/cjson_cbmc_entry.c" >"$tmp/b.log" 2>&1
  status=$?
  echo; echo "Input of $((size-1)) characters: exit status $status in $((SECONDS-start)) s. $(grep -E '^\*\* [0-9]+ of' "$tmp/b.log")"
  if grep -q ': FAILURE$' "$tmp/b.log"; then
    echo; echo '```'; grep ': FAILURE$' "$tmp/b.log" | sed -E 's/: FAILURE$//' | cut -c1-120 | head -25; echo '```'
  fi
done

[ "$run_stage" -eq 1 ] || { echo; echo "## Stage E: not run (needs --run)"; exit 0; }

echo; echo "## Stage E: sanitizers on the inputs shipped with cJSON"
# shellcheck disable=SC2086
gcc $CH_DYNAMIC_FLAGS -I"$src" "$CH_ROOT/audit/cjson_driver.c" "$target" -lm -o "$tmp/driver" || exit 2
total=0; stopped=0
: > "$tmp/runs"
for input in "$src"/tests/inputs/* "$src"/fuzzing/inputs/*; do
  [ -f "$input" ] || continue
  total=$((total+1))
  if ! timeout 30 "$tmp/driver" "$input" >/dev/null 2>"$tmp/e.log"; then
    stopped=$((stopped+1))
    echo "$(basename "$input"): $(grep -m1 -E 'ERROR: |runtime error' "$tmp/e.log" | cut -c1-90)" >> "$tmp/runs"
  fi
done
echo; echo "Inputs: $total. Stopped by a sanitizer: $stopped."
if [ -s "$tmp/runs" ]; then echo; echo '```'; cat "$tmp/runs"; echo '```'; fi
