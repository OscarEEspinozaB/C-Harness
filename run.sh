#!/usr/bin/env bash
# Measures every cell without and with the harness. Harness stages:
#   C = compile, V = verifier, S = symbols, B = bounded search, E = sanitized run.
set -u
. "$(cd "$(dirname "$0")" && pwd)/harness/stages.sh"
tmp=$(mktemp -d); trap 'rm -rf "$tmp"' EXIT
export ASAN_OPTIONS=detect_leaks=1
build_verifier || { echo "could not build the verifier" >&2; exit 2; }

measure_baseline() {
  gcc -std=c17 -w -I"$CH_ROOT/harness" -I"$(dirname "$1")" -include ch_headers.h -c "$1" -o "$tmp/b.o" 2>/dev/null || { echo C; return; }
  nm "$tmp/b.o" | grep -q ' T main$' || { echo -; return; }
  gcc "$tmp/b.o" -o "$tmp/b" 2>/dev/null || { echo C; return; }
  timeout 20 "$tmp/b" >/dev/null 2>&1; [ $? -eq 0 ] && echo - || echo E
}

echo "| Cell | Premise | Kind | Baseline | Harness | Verdict | Reason |"
echo "|---|---|---|---|---|---|---|"
passed=0; failed=0; limits=0
for cell in "$CH_ROOT"/cells/*.c; do
  name=$(basename "$cell" .c)
  premise=$(sed -n '1s/.*premise: \([^ ]*\).*/\1/p' "$cell")
  kind=$(sed -n '1s/.*kind: \([^ ]*\).*/\1/p' "$cell")
  expected=$(sed -n '2s/^\/\/ reason: //p' "$cell")
  baseline=$(measure_baseline "$cell"); measure_harness "$cell" > "$tmp/stage"; stage=$(cat "$tmp/stage"); reason=$(cat "$tmp/reason")
  case "$kind" in
    control) [ "$stage" = - ] && verdict=pass || verdict="**FAIL**" ;;
    limit)   [ "$stage" = - ] && verdict=limit || verdict="**REVIEW**" ;;
    *)
      if [ "$stage" = - ]; then verdict="**FAIL**"
      elif [ -n "$expected" ] && grep -qF -- "$expected" <<<"$reason"; then verdict=pass
      else verdict="**REASON**"; fi ;;
  esac
  case "$verdict" in pass) passed=$((passed+1));; limit) limits=$((limits+1));; *) failed=$((failed+1));; esac
  echo "| $name | $premise | $kind | $baseline | $stage | $verdict | ${reason:0:90} |"
done
echo
echo "Passed: $passed. Declared limits: $limits. Failed: $failed."
if [ -s "$tmp/inconclusive" ]; then
  echo "CBMC did not conclude on: $(paste -sd' ' "$tmp/inconclusive")"
else
  echo "CBMC concluded on every cell it analyzed."
fi
[ "$failed" -eq 0 ]
