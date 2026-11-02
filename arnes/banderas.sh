# Banderas del arnés. Se carga desde correr.sh.
CH_ESTATICAS="-std=c17 -pedantic-errors -O2 -Werror -Wall -Wextra -Wshadow -Wconversion
 -Wsign-conversion -Wcast-qual -Wcast-align=strict -Wwrite-strings -Wvla -Wformat=2
 -Wformat-nonliteral -Wstrict-prototypes -Wmissing-prototypes -Wmissing-declarations
 -Wredundant-decls -Wnested-externs -Wimplicit-fallthrough=5 -Wswitch-enum -Wnull-dereference
 -Wuse-after-free=3 -Wdangling-pointer=2 -Warray-bounds=2 -Wstringop-overflow=4 -Wrestrict
 -Wbad-function-cast -Wold-style-definition -Wfloat-equal -fstrict-flex-arrays=3 -fanalyzer"
CH_DINAMICAS="-std=c17 -O1 -g -fsanitize=address,undefined,bounds-strict,float-divide-by-zero
 -fno-sanitize-recover=all -fno-omit-frame-pointer -ftrivial-auto-var-init=pattern"
