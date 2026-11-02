#!/usr/bin/env bash
# Audits an unmodified project, one stage at a time, without stopping at the first finding.
# Usage: audit.sh cjson|lz4 [--run]
# Projects are downloaded, never vendored: they have their own licenses.
# By default nothing from the project is executed. --run adds the sanitizer stage,
# which compiles the project and runs it on sample inputs.
set -u
. "$(cd "$(dirname "$0")" && pwd)/../harness/stages.sh"
tmp=$(mktemp -d); trap 'rm -rf "$tmp"' EXIT
export ASAN_OPTIONS=detect_leaks=1
project=${1:-}
run_stage=0
[ "${2:-}" = "--run" ] && run_stage=1
case "$project" in
  cjson)
    name=cJSON; tag=v1.7.19; commit=c859b25da02955fef659d658b8f324b5cde87be3
    url=https://github.com/DaveGamble/cJSON; include_dir=.; target_file=cJSON.c
    inputs="tests/inputs fuzzing/inputs"; link="-lm"; cbmc_sizes="3 5" ;;
  lz4)
    name=lz4; tag=v1.10.0; commit=ebb370ca83af193212df4dcbadcc5d87bc0de2f0
    url=https://github.com/lz4/lz4; include_dir=lib; target_file=lib/lz4.c
    inputs="lib"; link=""; cbmc_sizes="3 20" ;;
  *) echo "usage: $0 cjson|lz4 [--run]" >&2; exit 2 ;;
esac
build_verifier || exit 2

repo="$CH_ROOT/bin/audit/$name"
[ -d "$repo" ] || git clone -q --depth 1 --branch "$tag" "$url" "$repo" || exit 2
if [ "$(git -C "$repo" rev-parse HEAD)" != "$commit" ]; then echo "unexpected $name commit" >&2; exit 2; fi
src="$repo/$include_dir"
target="$repo/$target_file"

echo "# $name $tag audit"
echo
echo "Commit: \`$commit\`. Lines of $(basename "$target"): $(wc -l < "$target")."

echo; echo "## Stage C: poison"
echo; echo '```'
# shellcheck disable=SC2086
gcc $CH_STATIC_FLAGS -I"$CH_ROOT/harness" -I"$src" -include ch_poison.h -fmax-errors=0 -c "$target" -o "$tmp/o.o" 2>&1 \
  | grep -E 'error: attempt to use poisoned' | sed -E 's/.*poisoned //' | sort | uniq -c | sort -rn
echo '```'

echo; echo "## Stage C: warnings without poison"
echo; echo '```'
# shellcheck disable=SC2086
gcc ${CH_STATIC_FLAGS/-Werror/} -Wno-error -I"$src" -c "$target" -o "$tmp/o.o" 2>&1 | grep 'warning:' | sed -E 's/.*\[(CWE-[0-9]+\] \[)?-W/-W/; s/\]$//' | sort | uniq -c | sort -rn
echo '```'

echo; echo "## Stage V: verifier"
echo; echo '```'
"$CH_VERIFIER" "$target" -I"$src" -I"$CH_ROOT/harness" -include ch_headers.h 2>&1 | sed -E 's#^[^:]*/##' | cut -c1-110 | head -40
echo '```'

echo; echo "## Stage S: symbols"
gcc -std=c17 -O0 -I"$src" -c "$target" -o "$tmp/s.o"
echo; echo "State: $(nm "$tmp/s.o" | grep -E ' [DBCGdbs] ' | cut -d' ' -f2- | paste -sd, | sed 's/,/, /g')"
echo; echo "External symbols outside the list: $(nm "$tmp/s.o" | awk '$1=="U"{print $2}' | grep -vxFf "$CH_ROOT/harness/allowed_symbols.txt" | paste -sd, | sed 's/,/, /g')"

echo; echo "## Stage B: CBMC on a short input of unknown contents"
for size in $cbmc_sizes; do
  start=$SECONDS
  # shellcheck disable=SC2086
  timeout 100 cbmc -I"$src" -DCH_INPUT_SIZE=$size --bounds-check --pointer-check --div-by-zero-check --unwind 8 --no-unwinding-assertions "$CH_ROOT/audit/${project}_cbmc_entry.c" >"$tmp/b.log" 2>&1
  status=$?
  echo; echo "Input of $((size-1)) characters: exit status $status in $((SECONDS-start)) s. $(grep -E '^\*\* [0-9]+ of' "$tmp/b.log")"
  if grep -q ': FAILURE$' "$tmp/b.log"; then
    echo; echo '```'; grep ': FAILURE$' "$tmp/b.log" | sed -E 's/: FAILURE$//' | cut -c1-120 | head -25; echo '```'
  fi
done

echo; echo "## Practice counts (exploratory, not a harness stage)"
echo; echo '```'
python3 "$CH_ROOT/audit/practices.py" "$target" -I"$src"
echo '```'

[ "$run_stage" -eq 1 ] || { echo; echo "## Stage E: not run (needs --run)"; exit 0; }

echo; echo "## Stage E: sanitizers on sample inputs"
# shellcheck disable=SC2086
gcc $CH_DYNAMIC_FLAGS -I"$src" "$CH_ROOT/audit/${project}_driver.c" "$target" $link -o "$tmp/driver" || exit 2
total=0; stopped=0
: > "$tmp/runs"
for input in $(for dir in $inputs; do printf '%s ' "$repo/$dir"/*; done); do
  [ -f "$input" ] || continue
  total=$((total+1))
  if ! timeout 30 "$tmp/driver" "$input" >/dev/null 2>"$tmp/e.log"; then
    stopped=$((stopped+1))
    echo "$(basename "$input"): $(grep -m1 -E 'ERROR: |runtime error' "$tmp/e.log" | cut -c1-90)" >> "$tmp/runs"
  fi
done
echo; echo "Inputs: $total. Stopped by a sanitizer: $stopped."
if [ -s "$tmp/runs" ]; then echo; echo '```'; cat "$tmp/runs"; echo '```'; fi
