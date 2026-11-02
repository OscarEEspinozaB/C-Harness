#!/usr/bin/env bash
# Instala las herramientas de C-Harness en Debian 13.
set -euo pipefail

echo "== Paquetes de apt =="
sudo apt update
sudo apt install -y clang clang-tidy libclang-dev python3-clang cbmc valgrind afl++ cppcheck opam

echo "== Frama-C con opam =="
[ -d "$HOME/.opam" ] || opam init --bare -y
opam update -y
opam switch list --short | grep -qx frama-c || opam switch create frama-c --empty -y
eval "$(opam env --switch=frama-c --set-switch)"
opam install -y --switch=frama-c frama-c alt-ergo

echo "== Verificación =="
fallos=0
probar() {
  local nombre=$1; shift
  if salida=$("$@" 2>&1); then
    printf 'OK    %-12s %s\n' "$nombre" "$(head -1 <<<"$salida")"
  else
    printf 'FALLA %-12s\n' "$nombre"
    fallos=$((fallos + 1))
  fi
}
probar clang      clang --version
probar clang-tidy clang-tidy --version
probar libclang   python3 -c "import clang.cindex as c; c.Index.create(); print('cargada')"
probar cbmc       cbmc --version
probar valgrind   valgrind --version
probar afl++      command -v afl-fuzz
probar cppcheck   cppcheck --version
probar frama-c    frama-c -version
probar alt-ergo   alt-ergo --version

echo "Herramientas con fallo: $fallos"
exit "$fallos"
