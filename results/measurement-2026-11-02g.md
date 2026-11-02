| Cell | Premise | Kind | Baseline | Harness | Verdict | Reason |
|---|---|---|---|---|---|---|
| agt1_constructor | EFE-1 | violation | - | C | pass | attempt to use poisoned "__attribute__" |
| agt1_hidden_initializer | EFE-1 | evasion | - | C | pass | attempt to use poisoned "__attribute__" |
| agt1_initializer_in_module | EFE-1 | evasion | - | C | pass | attempt to use poisoned "__attribute__" |
| agt3_dynamic_loading | EFE-2 | violation | - | C | pass | attempt to use poisoned "dlopen" |
| agt4_alias_declaration | EFE-3 | evasion | - | C | pass | attempt to use poisoned "__asm__" |
| agt4_system | EFE-3 | violation | - | C | pass | attempt to use poisoned "system" |
| agt4_token_paste_declaration | EFE-3 | evasion | - | C | pass | attempt to use poisoned "system" |
| agt4_unlisted_effect | EFE-3 | evasion | - | S | pass | symbol not allowed: unlink |
| base1_control_standard_headers | FUN-1 | control | - | - | pass |  |
| est2_control_const_pointer_table | EST-2 | control | - | - | pass |  |
| evasion_line_marker_header | ARN-1 | evasion | - | C | pass | style of line directive is a GCC extension |
| evasion_own_header | ARN-1 | evasion | - | C | pass | attempt to use poisoned "system_header" |
| evasion_silence_warning | ARN-1 | evasion | - | C | pass | attempt to use poisoned "diagnostic" |
| lim_adjacent_heap | LIM-1 | violation | - | B | pass | line 3 dereference failure: pointer outside object bounds in a[(signed long int)(argc + 3) |
| lim_computed_index | LIM-1 | violation | E | B | pass | line 3 array 'a' upper bound in a[(signed long int)(argc + 9)] |
| lim_constant_index | LIM-1 | violation | E | C | pass | stack-based buffer over-read [CWE-126] [-Werror=analyzer-out-of-bounds] |
| lim_inside_struct | LIM-1 | evasion | - | B | pass | line 8 dereference failure: pointer outside object bounds in p[(signed long int)((size_t)a |
| lim_jump_to_other_block | LIM-1 | evasion | - | B | pass | line 8 dereference failure: pointer outside object bounds in a[(signed long int)d] |
| lim_pointer_arithmetic | LIM-1 | evasion | - | B | pass | line 7 dereference failure: pointer outside object bounds in p[(signed long int)2] |
| mem1_cast_away_const | EST-1 | evasion | E | C | pass | cast discards ‘const’ qualifier from pointer target type [-Werror=cast-qual] |
| mem1_cast_away_const_via_integer | EST-1 | evasion | E | C | pass | write to ‘const’ object ‘K’ [-Werror=analyzer-write-to-const] |
| mem1_reassign_constant | EST-1 | violation | C | C | pass | assignment of read-only variable ‘K’ |
| mem2_control_parameter | EST-2 | control | - | - | pass |  |
| mem2_file_variable | EST-2 | violation | - | S | pass | state in the main file: total |
| mem2_static_local | EST-2 | evasion | - | S | pass | state in the main file: n.0 |
| mem3_control_module | EST-2 | control | - | - | pass |  |
| mem3_exported_variable | EST-2 | violation | - | S | pass | exported variable: shared |
| mem4_control_address_to_const_field | EST-3 | control | - | - | pass |  |
| mem4_control_const_read | EST-3 | control | - | - | pass |  |
| mem4_control_const_table_address | EST-3 | control | - | - | pass |  |
| mem4_indirect_pointer | EST-3 | limit | - | - | limit |  |
| mem4_output_via_parameter | EST-3 | evasion | - | V | pass | STATE-1: 'state' is function or module state and its address leaves with write access |
| mem4_pointer_to_state | EST-3 | evasion | - | V | pass | STATE-1: 'state' is function or module state and its address leaves with write access |
| mem4_static_array | EST-3 | evasion | - | V | pass | STATE-1: 'buffer' is function or module state and its address leaves with write access |
| mem5_control_c_form | INT-1 | control | - | - | pass |  |
| mem5_control_distinct_outputs | INT-1 | control | - | - | pass |  |
| mem5_control_fixed_callback | INT-1 | control | - | - | pass |  |
| mem5_control_header_prototype | INT-1 | control | - | - | pass |  |
| mem5_control_output_and_read | INT-1 | control | - | - | pass |  |
| mem5_control_writes_after_passing | INT-1 | control | - | - | pass |  |
| mem5_control_write_through_cast | INT-1 | control | - | - | pass |  |
| mem5_header_prototype_not_written | INT-1 | violation | - | V | pass | CONST-1: parameter 'value' of 'api_read' is never written: declare it 'const int *' |
| mem5_output_not_written | INT-1 | violation | - | V | pass | CONST-1: parameter 'p' of 'read_value' is never written: declare it 'const int *' |
| mem5_output_via_pointer_variable | INT-1 | limit | - | - | limit |  |
| mem5_prototype_not_written | INT-1 | violation | - | V | pass | CONST-1: parameter 'p' of 'read_value' is never written: declare it 'const int *' |
| mem5_same_output_restrict | INT-2 | violation | - | C | pass | passing argument 1 to ‘restrict’-qualified parameter aliases with argument 2 [-Werror= |
| mem5_same_output_twice | INT-2 | violation | - | V | pass | ALIAS-1: 'x' is passed twice as output in the same call |
| mem5_undeclared_call_stops_analysis | INT-1 | violation | C | C | pass | implicit declaration of function ‘undeclared_writer’ [-Wimplicit-function-declaration] |
| mem5_unmarked_call | INT-1 | violation | C | C | pass | passing argument 1 of ‘increment’ makes pointer from integer without a cast [-Wint-con |
| mem5_unmarked_signature | INT-1 | violation | C | C | pass | passing argument 1 of ‘increment’ makes integer from pointer without a cast [-Wint-con |
| mem6_block_does_not_leak | EST-4 | violation | C | C | pass | ‘x’ undeclared (first use in this function) |
| mem6_nested_function | EST-4 | violation | - | C | pass | ISO C forbids nested functions [-Wpedantic] |
| mem7_control_libc_name | INT-3 | control | - | - | pass |  |
| mem7_duplicate_parameters | INT-3 | violation | C | C | pass | redefinition of parameter ‘a’ |
| mem7_local_shadow | INT-3 | violation | - | C | pass | declaration of ‘x’ shadows a previous local [-Werror=shadow] |
| mem7_parameter_named_as_function | INT-3 | violation | - | V | pass | NAME-1: 'total' already names a function of this file |
| mem7_variable_named_as_function | INT-3 | violation | - | V | pass | NAME-1: 'datum' already names a function of this file |
| mem8_after_quarantine | VID-1 | limit | - | - | limit |  |
| mem8_control_goto_cleanup | VID-1 | control | - | - | pass |  |
| mem8_control_list | VID-1 | control | - | - | pass |  |
| mem8_control_release | VID-1 | control | - | - | pass |  |
| mem8_double_free | VID-1 | violation | E | C | pass | pointer ‘p’ used after ‘free’ [-Werror=use-after-free] |
| mem8_leak | VID-1 | violation | - | E | pass | detected memory leaks |
| mem8_opaque_use_after_free | VID-1 | evasion | E | C | pass | use after ‘free’ of ‘p’ [CWE-416] [-Werror=analyzer-use-after-free] |
| mem8_unexecuted_path | VID-1 | evasion | - | C | pass | use after ‘free’ of ‘p’ [CWE-416] [-Werror=analyzer-use-after-free] |
| mem8_use_after_free | VID-1 | violation | E | C | pass | pointer ‘p’ used after ‘free’ [-Werror=use-after-free] |
| typ_c_union | TIP-1 | control | - | - | pass |  |
| typ_integer_to_pointer | TIP-1 | evasion | - | B | pass | line 8 dereference failure: pointer outside object bounds in *((signed int *)addr) |
| typ_narrowing_conversion | TIP-1 | violation | - | C | pass | conversion from ‘int’ to ‘char’ may change value [-Werror=conversion] |
| typ_undeclared | TIP-1 | violation | C | C | pass | implicit declaration of function ‘undeclared_function’ [-Wimplicit-function-declaratio |
| typ_void_confusion | TIP-1 | limit | - | - | limit |  |
| ub_argc_overflow_within_bound | UB-1 | violation | - | B | pass | line 4 arithmetic overflow on signed * in argc * 3000 |
| ub_control_argc_plus_one | UB-1 | control | - | - | pass | warning, only above argc 1048576: arithmetic overflow on signed + in argc + 1 |
| ub_data_race | UB-1 | evasion | - | S | pass | symbol not allowed: pthread_create |
| ub_division_by_zero | UB-1 | violation | E | B | pass | line 3 division by zero in 1 / (argc - 1) |
| ub_infinite_recursion | UB-1 | violation | E | C | pass | infinite recursion detected [-Werror=infinite-recursion] |
| ub_integer_overflow | UB-1 | violation | - | B | pass | line 3 arithmetic overflow on signed + in x + argc |
| ub_returns_local | UB-1 | violation | E | C | pass | function returns address of local variable [-Werror=return-local-addr] |
| ub_uninitialized | UB-1 | violation | E | C | pass | ‘x’ is used uninitialized [-Werror=uninitialized] |

Passed: 75. Declared limits: 4. Failed: 0. With warnings: 3.
CBMC concluded on every cell it analyzed.
