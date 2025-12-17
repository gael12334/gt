/*
    Copyright © 2025 Gaël Fortier <gael.fortier.1@ens.etsmtl.ca>
*/

#include "cdocs/cdocs.h"
#include "test.h"


int main(int argc, char** argv) {

    _cdocs_fn_struct functions[] = {
        _cdocs_fn(
            _cdocs_fn_f(_gt_run, "Macro that execute a test declared by `_gt_test`."),
            _cdocs_fn_r(void, "N/A"),
            _cdocs_fn_p(fn, "The name of the function tested."),
            _cdocs_fn_p(opt_case, "The name of the test case. This parameter can be left empty.")
        ),

        _cdocs_fn(
            _cdocs_fn_f(_gt_test, "Template macro for the declaration of tests."),
            _cdocs_fn_r(void, "N/A"),
            _cdocs_fn_p(fn, "The name of the function tested."),
            _cdocs_fn_p(opt_case, "The name of the test case. This parameter can be left empty.")
        ), 

        _cdocs_fn(
            _cdocs_fn_f(_gt_passed, "Prints that the test has passed."),
            _cdocs_fn_r(void, "N/A"),
            _cdocs_fn_p(void, "No parameters.")
        ), 

        _cdocs_fn(
            _cdocs_fn_f(_gt_test_print, 
                "This is a wrapper of the function `printf`. "
                "This macro displays the location where it was used, then calls printf with provided parameters. "
                "This is useful for keeping track of the location of temporary debug prints. "),
            _cdocs_fn_r(void, "N/A"),
            _cdocs_fn_p(const char* format, "See the parameter `format` of function `printf`."),
            _cdocs_fn_p(..., "See the parameter `format` of function `printf`.")
        ), 

        // ------------------------ equality

        _cdocs_fn(
            _cdocs_fn_f(_gt_test_int_eq, "Asserts integer equality."),
            _cdocs_fn_r(void, "N/A"),
            _cdocs_fn_p(long _n1, "Left integer."),
            _cdocs_fn_p(long _n2, "Right integer.")
        ),

        _cdocs_fn(
            _cdocs_fn_f(_gt_test_float_eq, "Asserts floating point equality."),
            _cdocs_fn_r(void, "N/A"),
            _cdocs_fn_p(double _n1, "Left floating point value."),
            _cdocs_fn_p(double _n2, "Right floating point value.")
        ),

        _cdocs_fn(
            _cdocs_fn_f(_gt_test_ptr_eq, "Asserts pointer equality."),
            _cdocs_fn_r(void, "N/A"),
            _cdocs_fn_p(void* _n1, "Left pointer."),
            _cdocs_fn_p(void* _n2, "Right pointer.")
        ),

        _cdocs_fn(
            _cdocs_fn_f(_gt_test_str_eq, "Asserts string equality."),
            _cdocs_fn_r(void, "N/A"),
            _cdocs_fn_p(const char* _n1, "Left string."),
            _cdocs_fn_p(const char* _n2, "Right string.")
        ),

        // ------------------------ inequality

        _cdocs_fn(
            _cdocs_fn_f(_gt_test_int_neq, "Asserts integer inequality."),
            _cdocs_fn_r(void, "N/A"),
            _cdocs_fn_p(long _n1, "Left integer."),
            _cdocs_fn_p(long _n2, "Right integer.")
        ),

        _cdocs_fn(
            _cdocs_fn_f(_gt_test_float_neq, "Asserts floating point inequality."),
            _cdocs_fn_r(void, "N/A"),
            _cdocs_fn_p(double _n1, "Left floating point value."),
            _cdocs_fn_p(double _n2, "Right floating point value.")
        ),

        _cdocs_fn(
            _cdocs_fn_f(_gt_test_ptr_neq, "Asserts pointer inequality."),
            _cdocs_fn_r(void, "N/A"),
            _cdocs_fn_p(void* _n1, "Left pointer."),
            _cdocs_fn_p(void* _n2, "Right pointer.")
        ),

        _cdocs_fn(
            _cdocs_fn_f(_gt_test_str_neq, "Asserts string inequality."),
            _cdocs_fn_r(void, "N/A"),
            _cdocs_fn_p(const char* _n1, "Left string."),
            _cdocs_fn_p(const char* _n2, "Right string.")
        ),

        // ------------------------ superiority

        _cdocs_fn(
            _cdocs_fn_f(_gt_test_int_gt, "Asserts first integer greater than second integer."),
            _cdocs_fn_r(void, "N/A"),
            _cdocs_fn_p(long _n1, "Left integer."),
            _cdocs_fn_p(long _n2, "Right integer.")
        ),

        _cdocs_fn(
            _cdocs_fn_f(_gt_test_float_gt, "Asserts first floating point value greater than second floating point value."),
            _cdocs_fn_r(void, "N/A"),
            _cdocs_fn_p(double _n1, "Left floating point value."),
            _cdocs_fn_p(double _n2, "Right floating point value.")
        ),

        _cdocs_fn(
            _cdocs_fn_f(_gt_test_int_gte, "Asserts first integer greater than or equal second integer."),
            _cdocs_fn_r(void, "N/A"),
            _cdocs_fn_p(long _n1, "Left integer."),
            _cdocs_fn_p(long _n2, "Right integer.")
        ),

        _cdocs_fn(
            _cdocs_fn_f(_gt_test_float_gte, "Asserts first floating point value greater than or equal second floating point value."),
            _cdocs_fn_r(void, "N/A"),            
            _cdocs_fn_p(double _n1, "Left floating point value."),
            _cdocs_fn_p(double _n2, "Right floating point value.")
        ),

        // ------------------------ inferiority

        _cdocs_fn(
            _cdocs_fn_f(_gt_test_int_lt, "Asserts first integer lesser than second integer."),
            _cdocs_fn_r(void, "N/A"),
            _cdocs_fn_p(long _n1, "Left integer."),
            _cdocs_fn_p(long _n2, "Right integer.")
        ),

        _cdocs_fn(
            _cdocs_fn_f(_gt_test_float_lt, "Asserts first floating point value lesser than second floating point value."),
            _cdocs_fn_r(void, "N/A"),
            _cdocs_fn_p(double _n1, "Left floating point value."),
            _cdocs_fn_p(double _n2, "Right floating point value.")
        ),

        _cdocs_fn(
            _cdocs_fn_f(_gt_test_int_lte, "Asserts first integer lesser than or equal second integer."),
            _cdocs_fn_r(void, "N/A"),
            _cdocs_fn_p(long _n1, "Left integer."),
            _cdocs_fn_p(long _n2, "Right integer.")
        ),

        _cdocs_fn(
            _cdocs_fn_f(_gt_test_float_lte, "Asserts first floating point value lesser than or equal second floating point value."),
            _cdocs_fn_r(void, "N/A"),            
            _cdocs_fn_p(double _n1, "Left floating point value."),
            _cdocs_fn_p(double _n2, "Right floating point value.")
        ),

    };

    _cdocs_generate_documentation("gt", functions);
    return 0;
}
