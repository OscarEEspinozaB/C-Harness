# Etapas del arnés. Se carga desde correr.sh y autoverificar.sh.
# Bash solo coordina: lo que decide son GCC, nm, los sanitizadores y ch_verificar.
CH_RAIZ=$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)
. "$CH_RAIZ/arnes/banderas.sh"
CH_LLVM=/usr/lib/llvm-19
CH_VERIFICADOR="$CH_RAIZ/bin/ch_verificar"

construir_verificador() {
  mkdir -p "$CH_RAIZ/bin"
  gcc -std=c17 -O2 -isystem "$CH_LLVM/include" -I"$CH_RAIZ/arnes" -include ch_cabeceras.h \
    "$CH_RAIZ/verificador/ch_verificar.c" -o "$CH_VERIFICADOR" \
    -L"$CH_LLVM/lib" -lclang-19 -Wl,-rpath,"$CH_LLVM/lib"
}

# Devuelve 1 y escribe el motivo en $tmp/m si el objeto incumple una regla de símbolos.
simbolos() {
  local tabla externo
  tabla=$(nm "$1")
  if grep -qE ' [DBCG] ' <<<"$tabla"; then
    grep -m1 -E ' [DBCG] ' <<<"$tabla" | cut -d' ' -f2- | sed 's/^/variable exportada: /' > "$tmp/m"; return 1
  fi
  if grep -qE ' T main$' <<<"$tabla" && grep -qE ' [dbs] ' <<<"$tabla"; then
    grep -m1 -E ' [dbs] ' <<<"$tabla" | cut -d' ' -f2- | sed 's/^/estado en el archivo de main: /' > "$tmp/m"; return 1
  fi
  externo=$(awk '$1 == "U" {print $2}' <<<"$tabla" | grep -vxFf "$CH_RAIZ/arnes/simbolos_permitidos.txt" | grep -vE "${CH_SIMBOLOS_EXTRA:-^$}" | head -1)
  if [ -n "$externo" ]; then
    echo "símbolo externo no permitido: $externo" > "$tmp/m"; return 1
  fi
  return 0
}

# Escribe la etapa que detuvo el archivo (C, V, S, E) o «-». Motivo en $tmp/m.
# Opciones adicionales: CH_INCLUSION (compilación) y CH_ENLACE, CH_ARGS_EJECUCION.
medir_arnes() {
  : > "$tmp/m"
  # shellcheck disable=SC2086
  if ! gcc $CH_ESTATICAS -I"$CH_RAIZ/arnes" -I"$(dirname "$1")" ${CH_INCLUSION:-} -include ch_veneno.h -c "$1" -o "$tmp/a.o" 2>"$tmp/c.log"; then
    grep -m1 'error' "$tmp/c.log" | sed 's/^[^ ]* error: //; s/|/\\|/g' | cut -c1-90 > "$tmp/m"; echo C; return
  fi
  # shellcheck disable=SC2086
  "$CH_VERIFICADOR" "$1" -I"$CH_RAIZ/arnes" -I"$(dirname "$1")" ${CH_INCLUSION:-} -include ch_cabeceras.h >"$tmp/v.log" 2>"$tmp/v.err"
  local r=$?
  if [ $r -ne 0 ]; then
    if [ $r -eq 1 ]; then sed -n '1s/^[^ ]* //p' "$tmp/v.log" | cut -c1-90 > "$tmp/m"
    else head -1 "$tmp/v.err" | cut -c1-90 > "$tmp/m"; fi
    echo V; return
  fi
  # shellcheck disable=SC2086
  gcc -std=c17 -O0 -I"$CH_RAIZ/arnes" -I"$(dirname "$1")" ${CH_INCLUSION:-} -include ch_veneno.h -c "$1" -o "$tmp/s.o" 2>/dev/null
  simbolos "$tmp/s.o" || { echo S; return; }
  nm "$tmp/s.o" | grep -q ' T main$' || { echo -; return; }
  # shellcheck disable=SC2086
  if ! gcc $CH_DINAMICAS -I"$CH_RAIZ/arnes" -I"$(dirname "$1")" ${CH_INCLUSION:-} -include ch_veneno.h "$1" -o "$tmp/e" ${CH_ENLACE:-} 2>"$tmp/c.log"; then
    grep -m1 'error' "$tmp/c.log" | cut -c1-90 > "$tmp/m"; echo C; return
  fi
  # shellcheck disable=SC2086
  timeout 120 "$tmp/e" ${CH_ARGS_EJECUCION:-} >/dev/null 2>"$tmp/e.log"; r=$?
  [ $r -eq 0 ] && { echo -; return; }
  grep -m1 -E 'ERROR: |runtime error' "$tmp/e.log" | sed -E 's/.*(ERROR: [A-Za-z]+: |runtime error: )//' | cut -c1-60 > "$tmp/m"
  [ -s "$tmp/m" ] || echo "código de salida $r" > "$tmp/m"
  echo E
}
