#!/usr/bin/env bash
# AUT-1: C-Harness pasa por C-Harness. Una mutación comprueba que el paso no es trivial.
set -u
. "$(cd "$(dirname "$0")" && pwd)/arnes/etapas.sh"
tmp=$(mktemp -d); trap 'rm -rf "$tmp"' EXIT
export ASAN_OPTIONS=detect_leaks=1
construir_verificador || { echo "no se pudo construir el verificador" >&2; exit 2; }

fuente="$CH_RAIZ/verificador/ch_verificar.c"
CH_INCLUSION="-isystem $CH_LLVM/include -include clang-c/Index.h"
CH_ENLACE="-L$CH_LLVM/lib -lclang-19 -Wl,-rpath,$CH_LLVM/lib"
CH_ARGS_EJECUCION="$fuente -I$CH_RAIZ/arnes -isystem $CH_LLVM/include -include ch_cabeceras.h"
CH_SIMBOLOS_EXTRA='^clang_'

etapa=$(medir_arnes "$fuente")
if [ "$etapa" != - ]; then
  echo "AUT-1 no se sostiene: etapa $etapa: $(cat "$tmp/m")"
  exit 1
fi
echo "El verificador pasa por compilación, verificador, símbolos y ejecución con sanitizadores."

mutado="$tmp/mutado.c"
sed 's/static void informar(const char \*ruta, const struct candidato \*c)/static void informar(const char *ruta, struct candidato *c)/' "$fuente" > "$mutado"
if cmp -s "$fuente" "$mutado"; then echo "la mutación ya no aplica: revísala" >&2; exit 2; fi
etapa=$(medir_arnes "$mutado")
if [ "$etapa" = - ]; then
  echo "AUT-1 no se sostiene: la mutación pasó sin detección"
  exit 1
fi
echo "Quitar un const del verificador lo detiene en la etapa $etapa: $(cat "$tmp/m")"
