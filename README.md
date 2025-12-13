This is a micro C test library with pretty output.

Assertion macros: 

| Macro                        | Description                                            |
| ---------------------------- | ------------------------------------------------------ |
| `_gt_test_int_eq(n1, n2)`    | Asserts equality of 2 integer values.                  |
| `_gt_test_ptr_eq(n1, n2)`    | Asserts equality of 2 pointer values.                  |
| `_gt_test_float_eq(n1, n2)`  | Asserts equality of 2 real values.                     |
| `_gt_test_str_eq(n1, n2)`    | Asserts equality of 2 string values.                   |
| `_gt_test_int_neq(n1, n2)`   | Asserts inequality of 2 integer values.                |
| `_gt_test_ptr_neq(n1, n2)`   | Asserts inequality of 2 pointer values.                |
| `_gt_test_float_neq(n1, n2)` | Asserts inequality of 2 real values.                   |
| `_gt_test_str_neq(n1, n2)`   | Asserts inequality of 2 string values.                 |
| `_gt_test_int_gt(n1, n2)`    | Asserts n1 greater than n2 for integer values          |
| `_gt_test_float_gt(n1, n2)`  | Asserts n1 greater than n2 for real values             |
| `_gt_test_int_lt(n1, n2)`    | Asserts n1 lesser than n2 for integer values           |
| `_gt_test_float_lt(n1, n2)`  | Asserts n1 lesser than n2 for real values              |
| `_gt_test_int_gte(n1, n2)`   | Asserts n1 greater or equal to n2 for integer values   |
| `_gt_test_float_gte(n1, n2)` | Asserts n1 greater or equal to than n2 for real values |
| `_gt_test_int_lte(n1, n2)`   | Asserts n1 lesser or equal to n2 for integer values    |
| `_gt_test_float_lte(n1, n2)` | Asserts n1 lesser or equal to than n2 for real values  |

Debug print macro: 

`_gt_test_print(format, ...)` : Shows the function and line of usage and makes a box arround diplayed text.
