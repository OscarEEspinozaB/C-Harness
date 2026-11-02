#!/usr/bin/env bash
# Mide cada celda sin arnés y con arnés. Etapas del arnés:
#   C = compilación, S = símbolos (nm), E = ejecución con sanitizadores.
set -u
raiz=$(cd "$(dirname "$0")" && pwd)
. "$raiz/arnes/banderas.sh"
tmp=$(mktemp -d); trap 'rm -rf "$tmp"' EXIT
export ASAN_OPTIONS=detect_leaks=1

simbolos() { # falla si hay estado mutable prohibido
  local tabla; tabla=$(nm "$1")
  grep -qE ' [DBCG] ' <<<"$tabla" && return 1            # variable exportada (MEM-3, MEM-4)
  if grep -qE ' T main$' <<<"$tabla"; then
    grep -qE ' [dbs] ' <<<"$tabla" && return 1           # estado en el archivo de main (MEM-2)
  fi
  return 0
}

medir_base() {
  gcc -std=c17 -w -I"$raiz/arnes" -include ch_cabeceras.h -c "$1" -o "$tmp/b.o" 2>/dev/null || { echo C; return; }
  nm "$tmp/b.o" | grep -q ' T main$' || { echo -; return; }
  gcc "$tmp/b.o" -o "$tmp/b" 2>/dev/null || { echo C; return; }
  timeout 20 "$tmp/b" >/dev/null 2>&1; [ $? -eq 0 ] && echo - || echo E
}

medir_arnes() {
  motivo=""
  echo -n > "$tmp/m"
  # shellcheck disable=SC2086
  gcc $CH_ESTATICAS -I"$raiz/arnes" -include ch_veneno.h -c "$1" -o "$tmp/a.o" 2>"$tmp/c.log" || { grep -m1 'error' "$tmp/c.log" | sed 's/^[^ ]* error: //; s/|/\\|/g' | cut -c1-90 > "$tmp/m"; echo C; return; }
  # shellcheck disable=SC2086
  gcc -std=c17 -O0 -I"$raiz/arnes" -include ch_veneno.h -c "$1" -o "$tmp/s.o" 2>/dev/null
  simbolos "$tmp/s.o" || { nm "$tmp/s.o" | grep -E ' [dbsDBCG] ' | head -1 | cut -d' ' -f2- > "$tmp/m"; echo S; return; }
  nm "$tmp/s.o" | grep -q ' T main$' || { echo -; return; }
  # shellcheck disable=SC2086
  gcc $CH_DINAMICAS -I"$raiz/arnes" -include ch_veneno.h "$1" -o "$tmp/e" 2>/dev/null || { echo C; return; }
  timeout 60 "$tmp/e" >/dev/null 2>"$tmp/e.log"; r=$?
  [ $r -eq 0 ] && { echo -; return; }
  grep -m1 -E 'ERROR: |runtime error' "$tmp/e.log" | sed -E 's/.*(ERROR: [A-Za-z]+: |runtime error: )//' | cut -c1-60 > "$tmp/m"
  [ -s "$tmp/m" ] || echo "código de salida $r" > "$tmp/m"
  echo E
}

echo "| Celda | Premisa | Tipo | Sin arnés | Con arnés | Veredicto | Motivo |"
echo "|---|---|---|---|---|---|---|"
ok=0; fallo=0
for c in "$raiz"/celdas/*.c; do
  n=$(basename "$c" .c)
  premisa=$(sed -n '1s/.*premisa: \([^ ]*\).*/\1/p' "$c")
  tipo=$(sed -n '1s/.*tipo: \([^ ]*\).*/\1/p' "$c")
  b=$(medir_base "$c"); a=$(medir_arnes "$c"); m=$(cat "$tmp/m")
  if { [ "$tipo" = control ] && [ "$a" = - ]; } || { [ "$tipo" != control ] && [ "$a" != - ]; }; then
    v=cumple; ok=$((ok+1))
  else
    v="**FALLA**"; fallo=$((fallo+1))
  fi
  echo "| $n | $premisa | $tipo | $b | $a | $v | $m |"
done
echo
echo "Cumplen: $ok. Fallan: $fallo."
