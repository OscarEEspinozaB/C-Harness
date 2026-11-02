| Cell | Premise | Kind | Baseline | Harness | Verdict | Reason |
|---|---|---|---|---|---|---|
| agt1_constructor | AGT-1 | violation | - | C | pass | attempt to use poisoned "__attribute__" |
| agt1_hidden_initializer | AGT-1 | evasion | - | C | pass | attempt to use poisoned "__attribute__" |
| agt1_initializer_in_module | AGT-1 | evasion | - | C | pass | attempt to use poisoned "__attribute__" |
| agt3_dynamic_loading | AGT-3 | violation | - | C | pass | attempt to use poisoned "dlopen" |
| agt4_alias_declaration | AGT-4 | evasion | - | C | pass | attempt to use poisoned "__asm__" |
| agt4_system | AGT-4 | violation | - | C | pass | attempt to use poisoned "system" |
| agt4_token_paste_declaration | AGT-4 | evasion | - | C | pass | attempt to use poisoned "system" |
| agt4_unlisted_effect | AGT-4 | evasion | - | S | pass | symbol not allowed: unlink |
| evasion_line_marker_header | ARN | evasion | - | C | pass | style of line directive is a GCC extension |
| evasion_own_header | ARN | evasion | - | C | pass | attempt to use poisoned "system_header" |
| evasion_silence_warning | ARN | evasion | - | C | pass | attempt to use poisoned "diagnostic" |
| lim_adjacent_heap | LIM | violation | - | B | pass | line 3 arithmetic overflow on signed + in argc + 3;line 3 dereference failure: pointer out |
| lim_computed_index | LIM | violation | E | B | pass | line 3 array 'a' lower bound in a[(signed long int)(argc + 9)];line 3 array 'a' upper boun |
| lim_constant_index | LIM | violation | E | C | pass | stack-based buffer over-read [CWE-126] [-Werror=analyzer-out-of-bounds] |
| lim_inside_struct | LIM | evasion | - | B | pass | line 8 dereference failure: pointer outside object bounds in p[(signed long int)((size_t)a |
| lim_jump_to_other_block | LIM | evasion | - | B | pass | line 8 dereference failure: pointer outside object bounds in a[(signed long int)d] |
| lim_pointer_arithmetic | LIM | evasion | - | B | pass | line 7 dereference failure: pointer outside object bounds in p[(signed long int)2] |
| mem1_cast_away_const | MEM-1 | evasion | E | C | pass | cast discards ‘const’ qualifier from pointer target type [-Werror=cast-qual] |
| mem1_cast_away_const_via_integer | MEM-1 | evasion | E | C | pass | write to ‘const’ object ‘K’ [-Werror=analyzer-write-to-const] |
| mem1_reassign_constant | MEM-1 | violation | C | C | pass | assignment of read-only variable ‘K’ |
| mem2_control_parameter | MEM-2 | control | - | - | pass |  |
| mem2_file_variable | MEM-2 | violation | - | S | pass | state in the main file: b total |
| mem2_static_local | MEM-2 | evasion | - | S | pass | state in the main file: b n.0 |
| mem3_control_module | MEM-3 | control | - | - | pass |  |
| mem3_exported_variable | MEM-3 | violation | - | S | pass | exported variable: D shared |
| mem4_control_const_read | MEM-4 | control | - | - | pass |  |
| mem4_indirect_pointer | MEM-4 | limit | - | - | limit |  |
| mem4_output_via_parameter | MEM-4 | evasion | - | V | pass | STATE-1: 'state' is function or module state and its address leaves with write access |
| mem4_pointer_to_state | MEM-4 | evasion | - | V | pass | STATE-1: 'state' is function or module state and its address leaves with write access |
| mem4_static_array | MEM-4 | evasion | - | V | pass | STATE-1: 'buffer' is function or module state and its address leaves with write access |
| mem5_control_c_form | MEM-5 | control | - | - | pass |  |
| mem5_control_distinct_outputs | MEM-5 | control | - | - | pass |  |
| mem5_control_fixed_callback | MEM-5 | control | - | - | pass |  |
| mem5_control_output_and_read | MEM-5 | control | - | - | pass |  |
| mem5_control_writes_after_passing | MEM-5 | control | - | - | pass |  |
| mem5_output_not_written | MEM-5 | violation | - | V | pass | CONST-1: parameter 'p' of 'read_value' is never written: declare it 'const int *' |
| mem5_output_via_pointer_variable | MEM-5 | limit | - | - | limit |  |
| mem5_prototype_not_written | MEM-5 | violation | - | V | pass | CONST-1: parameter 'p' of 'read_value' is never written: declare it 'const int *' |
| mem5_same_output_restrict | MEM-5 | violation | - | C | pass | passing argument 1 to ‘restrict’-qualified parameter aliases with argument 2 [-Werror= |
| mem5_same_output_twice | MEM-5 | violation | - | V | pass | ALIAS-1: 'x' is passed twice as output in the same call |
| mem5_unmarked_call | MEM-5 | violation | C | C | pass | passing argument 1 of ‘increment’ makes pointer from integer without a cast [-Wint-con |
| mem5_unmarked_signature | MEM-5 | violation | C | C | pass | passing argument 1 of ‘increment’ makes integer from pointer without a cast [-Wint-con |
| mem6_block_does_not_leak | MEM-6 | violation | C | C | pass | ‘x’ undeclared (first use in this function) |
| mem6_nested_function | MEM-6 | violation | - | C | pass | ISO C forbids nested functions [-Wpedantic] |
| mem7_control_libc_name | MEM-7 | control | - | - | pass |  |
| mem7_duplicate_parameters | MEM-7 | violation | C | C | pass | redefinition of parameter ‘a’ |
| mem7_local_shadow | MEM-7 | violation | - | C | pass | declaration of ‘x’ shadows a previous local [-Werror=shadow] |
| mem7_parameter_named_as_function | MEM-7 | violation | - | V | pass | NAME-1: 'total' already names a function of this file |
| mem7_variable_named_as_function | MEM-7 | violation | - | V | pass | NAME-1: 'datum' already names a function of this file |
| mem8_after_quarantine | MEM-8 | limit | - | - | limit |  |
| mem8_control_goto_cleanup | MEM-8 | control | - | - | pass |  |
| mem8_control_list | MEM-8 | control | - | - | pass |  |
| mem8_control_release | MEM-8 | control | - | - | pass |  |
| mem8_double_free | MEM-8 | violation | E | C | pass | pointer ‘p’ used after ‘free’ [-Werror=use-after-free] |
| mem8_leak | MEM-8 | violation | - | E | pass | detected memory leaks |
| mem8_opaque_use_after_free | MEM-8 | evasion | E | C | pass | use after ‘free’ of ‘p’ [CWE-416] [-Werror=analyzer-use-after-free] |
| mem8_unexecuted_path | MEM-8 | evasion | - | C | pass | use after ‘free’ of ‘p’ [CWE-416] [-Werror=analyzer-use-after-free] |
| mem8_use_after_free | MEM-8 | violation | E | C | pass | pointer ‘p’ used after ‘free’ [-Werror=use-after-free] |
| typ_c_union | TIP | control | - | - | pass |  |
| typ_integer_to_pointer | TIP | evasion | - | B | pass | line 8 dereference failure: pointer outside object bounds in *((signed int *)addr) |
| typ_narrowing_conversion | TIP | violation | - | C | pass | conversion from ‘int’ to ‘char’ may change value [-Werror=conversion] |
| typ_undeclared | TIP | violation | C | C | pass | implicit declaration of function ‘undeclared_function’ [-Wimplicit-function-declaratio |
| typ_void_confusion | TIP | limit | - | - | limit |  |
| ub_control_argc_plus_one | UB | control | - | B | **FAIL** | line 3 arithmetic overflow on signed + in argc + 1 |
| ub_data_race | UB | evasion | - | S | pass | symbol not allowed: pthread_create |
| ub_division_by_zero | UB | violation | E | B | pass | line 3 division by zero in 1 / (argc - 1) |
| ub_infinite_recursion | UB | violation | E | C | pass | infinite recursion detected [-Werror=infinite-recursion] |
| ub_integer_overflow | UB | violation | - | B | pass | line 3 arithmetic overflow on signed + in x + argc |
| ub_returns_local | UB | violation | E | C | pass | function returns address of local variable [-Werror=return-local-addr] |
| ub_uninitialized | UB | violation | E | C | pass | ‘x’ is used uninitialized [-Werror=uninitialized] |

Passed: 65. Declared limits: 4. Failed: 1.
CBMC concluded on every cell it analyzed.
