#!/usr/bin/env bash
# Installs the C-Harness tools on Debian 13.
set -euo pipefail

echo "== apt packages =="
sudo apt update
sudo apt install -y clang clang-tidy libclang-dev python3-clang cbmc valgrind afl++ cppcheck opam

echo "== Frama-C with opam =="
[ -d "$HOME/.opam" ] || opam init --bare -y
opam update -y
opam switch list --short | grep -qx frama-c || opam switch create frama-c --empty -y
eval "$(opam env --switch=frama-c --set-switch)"
opam install -y --switch=frama-c frama-c alt-ergo

echo "== Verification =="
failures=0
check_tool() {
  local name=$1; shift
  if output=$("$@" 2>&1); then
    printf 'OK    %-12s %s\n' "$name" "$(head -1 <<<"$output")"
  else
    printf 'FAIL  %-12s\n' "$name"
    failures=$((failures + 1))
  fi
}
check_tool clang      clang --version
check_tool clang-tidy clang-tidy --version
check_tool libclang   python3 -c "import clang.cindex as c; c.Index.create(); print('loaded')"
check_tool cbmc       cbmc --version
check_tool valgrind   valgrind --version
check_tool afl++      command -v afl-fuzz
check_tool cppcheck   cppcheck --version
check_tool frama-c    frama-c -version
check_tool alt-ergo   alt-ergo --version

echo "Tools that failed: $failures"
exit "$failures"
