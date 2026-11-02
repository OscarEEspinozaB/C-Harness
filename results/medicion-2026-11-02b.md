| Celda | Premisa | Tipo | Sin arnés | Con arnés | Veredicto | Motivo |
|---|---|---|---|---|---|---|
| agt1_constructor | AGT-1 | violacion | - | C | cumple | attempt to use poisoned "__attribute__" |
| agt1_inicializador_en_modulo | AGT-1 | evasion | - | C | cumple | attempt to use poisoned "__attribute__" |
| agt1_inicializador_oculto | AGT-1 | evasion | - | C | cumple | attempt to use poisoned "__attribute__" |
| agt3_carga_dinamica | AGT-3 | violacion | - | C | cumple | attempt to use poisoned "dlopen" |
| agt4_declaracion_con_alias | AGT-4 | evasion | - | C | cumple | attempt to use poisoned "__asm__" |
| agt4_declaracion_por_atajo | AGT-4 | evasion | - | C | cumple | attempt to use poisoned "system" |
| agt4_efecto_no_listado | AGT-4 | evasion | - | S | cumple | símbolo externo no permitido: unlink |
| agt4_system | AGT-4 | violacion | - | C | cumple | attempt to use poisoned "system" |
| evasion_cabecera_marca_linea | ARN | evasion | - | C | cumple | style of line directive is a GCC extension |
| evasion_cabecera_propia | ARN | evasion | - | C | cumple | attempt to use poisoned "system_header" |
| evasion_silenciar_diagnostico | ARN | evasion | - | C | cumple | attempt to use poisoned "diagnostic" |
| lim_aritmetica_puntero | LIM | limite | - | - | límite |  |
| lim_dentro_de_estructura | LIM | limite | - | - | límite |  |
| lim_heap_contiguo | LIM | violacion | - | E | cumple | load of address 0x502000000020 with insufficient space for a |
| lim_indice_calculado | LIM | violacion | E | E | cumple | index 10 out of bounds for type 'int [4]' |
| lim_indice_constante | LIM | violacion | E | C | cumple | stack-based buffer over-read [CWE-126] [-Werror=analyzer-out-of-bounds] |
| lim_salto_a_otro_bloque | LIM | evasion | - | E | cumple | store to address 0x506000000080 with insufficient space for  |
| mem1_quitar_const | MEM-1 | evasion | E | C | cumple | cast discards ‘const’ qualifier from pointer target type [-Werror=cast-qual] |
| mem1_quitar_const_por_entero | MEM-1 | evasion | E | C | cumple | write to ‘const’ object ‘K’ [-Werror=analyzer-write-to-const] |
| mem1_reasignar_constante | MEM-1 | violacion | C | C | cumple | assignment of read-only variable ‘K’ |
| mem2_control_parametro | MEM-2 | control | - | - | cumple |  |
| mem2_static_local | MEM-2 | evasion | - | S | cumple | estado en el archivo de main: b n.0 |
| mem2_variable_de_archivo | MEM-2 | violacion | - | S | cumple | estado en el archivo de main: b total |
| mem3_control_modulo | MEM-3 | control | - | - | cumple |  |
| mem3_variable_exportada | MEM-3 | violacion | - | S | cumple | variable exportada: D compartida |
| mem4_puntero_al_estado | MEM-4 | evasion | - | - | **FALLA** |  |
| mem5_control_callback_fijo | MEM-5 | control | - | - | cumple |  |
| mem5_control_escribe_tras_pasar | MEM-5 | control | - | - | cumple |  |
| mem5_control_forma_c | MEM-5 | control | - | - | cumple |  |
| mem5_firma_sin_marca | MEM-5 | violacion | C | C | cumple | passing argument 1 of ‘sube’ makes integer from pointer without a cast [-Wint-conversi |
| mem5_llamada_sin_marca | MEM-5 | violacion | C | C | cumple | passing argument 1 of ‘sube’ makes pointer from integer without a cast [-Wint-conversi |
| mem5_misma_salida_dos_veces | MEM-5 | violacion | - | - | **FALLA** |  |
| mem5_misma_salida_restrict | MEM-5 | violacion | - | C | cumple | passing argument 1 to ‘restrict’-qualified parameter aliases with argument 2 [-Werror= |
| mem5_prototipo_no_escrita | MEM-5 | violacion | - | V | cumple | CONST-1: el parámetro 'p' de 'lee' no se escribe: decláralo 'const int *' |
| mem5_salida_no_escrita | MEM-5 | violacion | - | V | cumple | CONST-1: el parámetro 'p' de 'lee' no se escribe: decláralo 'const int *' |
| mem5_salida_por_variable_puntero | MEM-5 | limite | - | - | límite |  |
| mem6_bloque_no_filtra | MEM-6 | violacion | C | C | cumple | ‘x’ undeclared (first use in this function) |
| mem6_funcion_anidada | MEM-6 | violacion | - | C | cumple | ISO C forbids nested functions [-Wpedantic] |
| mem7_parametros_iguales | MEM-7 | violacion | C | C | cumple | redefinition of parameter ‘a’ |
| mem7_sombra_local | MEM-7 | violacion | - | C | cumple | declaration of ‘x’ shadows a previous local [-Werror=shadow] |
| mem7_variable_como_funcion | MEM-7 | violacion | - | - | **FALLA** |  |
| mem8_camino_no_ejecutado | MEM-8 | evasion | - | C | cumple | use after ‘free’ of ‘p’ [CWE-416] [-Werror=analyzer-use-after-free] |
| mem8_control_goto_limpieza | MEM-8 | control | - | - | cumple |  |
| mem8_control_liberacion | MEM-8 | control | - | - | cumple |  |
| mem8_control_lista | MEM-8 | control | - | - | cumple |  |
| mem8_doble_free | MEM-8 | violacion | E | C | cumple | pointer ‘p’ used after ‘free’ [-Werror=use-after-free] |
| mem8_fuga | MEM-8 | violacion | - | E | cumple | detected memory leaks |
| mem8_tras_cuarentena | MEM-8 | limite | - | - | límite |  |
| mem8_uso_tras_free | MEM-8 | violacion | E | C | cumple | pointer ‘p’ used after ‘free’ [-Werror=use-after-free] |
| mem8_uso_tras_free_opaco | MEM-8 | evasion | E | C | cumple | use after ‘free’ of ‘p’ [CWE-416] [-Werror=analyzer-use-after-free] |
| tip_confusion_void | TIP | limite | - | - | límite |  |
| tip_conversion_estrecha | TIP | violacion | - | C | cumple | conversion from ‘int’ to ‘char’ may change value [-Werror=conversion] |
| tip_entero_a_puntero | TIP | limite | - | - | límite |  |
| tip_sin_declarar | TIP | violacion | C | C | cumple | implicit declaration of function ‘no_declarada’ [-Wimplicit-function-declaration] |
| tip_union_de_c | TIP | control | - | - | cumple |  |
| ub_carrera_de_datos | UB | evasion | - | S | cumple | símbolo externo no permitido: pthread_create |
| ub_desborde_entero | UB | violacion | - | E | cumple | signed integer overflow: 1 + 2147483647 cannot be represente |
| ub_devuelve_local | UB | violacion | E | C | cumple | function returns address of local variable [-Werror=return-local-addr] |
| ub_division_cero | UB | violacion | E | E | cumple | division by zero |
| ub_recursion_infinita | UB | violacion | E | C | cumple | infinite recursion detected [-Werror=infinite-recursion] |
| ub_sin_inicializar | UB | violacion | E | C | cumple | ‘x’ is used uninitialized [-Werror=uninitialized] |

Cumplen: 52. Límites declarados: 6. Fallan: 3.
