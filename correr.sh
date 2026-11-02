#!/usr/bin/env bash
# Mide cada celda sin arnés y con arnés. Etapas del arnés:
#   C = compilación, V = verificador, S = símbolos, B = búsqueda acotada, E = ejecución con sanitizadores.
set -u
. "$(cd "$(dirname "$0")" && pwd)/arnes/etapas.sh"
tmp=$(mktemp -d); trap 'rm -rf "$tmp"' EXIT
export ASAN_OPTIONS=detect_leaks=1
construir_verificador || { echo "no se pudo construir el verificador" >&2; exit 2; }

medir_base() {
  gcc -std=c17 -w -I"$CH_RAIZ/arnes" -I"$(dirname "$1")" -include ch_cabeceras.h -c "$1" -o "$tmp/b.o" 2>/dev/null || { echo C; return; }
  nm "$tmp/b.o" | grep -q ' T main$' || { echo -; return; }
  gcc "$tmp/b.o" -o "$tmp/b" 2>/dev/null || { echo C; return; }
  timeout 20 "$tmp/b" >/dev/null 2>&1; [ $? -eq 0 ] && echo - || echo E
}

echo "| Celda | Premisa | Tipo | Sin arnés | Con arnés | Veredicto | Motivo |"
echo "|---|---|---|---|---|---|---|"
cumple=0; falla=0; limite=0
for c in "$CH_RAIZ"/celdas/*.c; do
  n=$(basename "$c" .c)
  premisa=$(sed -n '1s/.*premisa: \([^ ]*\).*/\1/p' "$c")
  tipo=$(sed -n '1s/.*tipo: \([^ ]*\).*/\1/p' "$c")
  esperado=$(sed -n '2s/^\/\/ motivo: //p' "$c")
  b=$(medir_base "$c"); medir_arnes "$c" > "$tmp/a"; a=$(cat "$tmp/a"); m=$(cat "$tmp/m")
  case "$tipo" in
    control) [ "$a" = - ] && v=cumple || v="**FALLA**" ;;
    limite)  [ "$a" = - ] && v=límite || v="**REVISAR**" ;;
    *)
      if [ "$a" = - ]; then v="**FALLA**"
      elif [ -n "$esperado" ] && grep -qF -- "$esperado" <<<"$m"; then v=cumple
      else v="**MOTIVO**"; fi ;;
  esac
  case "$v" in cumple) cumple=$((cumple+1));; límite) limite=$((limite+1));; *) falla=$((falla+1));; esac
  echo "| $n | $premisa | $tipo | $b | $a | $v | ${m:0:90} |"
done
echo
echo "Cumplen: $cumple. Límites declarados: $limite. Fallan: $falla."
if [ -s "$tmp/sin_concluir" ]; then
  echo "CBMC no concluyó en: $(paste -sd' ' "$tmp/sin_concluir")"
else
  echo "CBMC concluyó en todas las celdas que analizó."
fi
[ "$falla" -eq 0 ]
