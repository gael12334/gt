/*
    Copyright © 2025 Gaël Fortier <gael.fortier.1@ens.etsmtl.ca>
*/

#include "calc.h"
#include "gt/test.h"

_gt_test(calc_even, ) {
    // arrange
    int A = 4;
    int B = 7;

    // act
    int RA = calc_even(A);
    int RB = calc_even(B);

    // assert
    _gt_test_int_eq(RA, 1);
    _gt_test_int_eq(RB, 0);
    _gt_passed();
}

_gt_test(calc_median, ) {
    // arrange
    const float set_odd[] = {1.f, 3.f, 5.f, 9.f, 10.f};
    const unsigned size_odd = sizeof(set_odd) / sizeof(set_odd[0]);

    const float set_even[] = {1.f, 3.f, 5.f, 9.f, 10.f, 15.f};
    const unsigned size_even = sizeof(set_even) / sizeof(set_even[0]);

    float R_odd;
    float R_even;

    // act
    R_odd = calc_median(set_odd, size_odd);
    R_even = calc_median(set_even, size_even);

    // assert
    _gt_test_float_eq(R_odd, 5.f);
    _gt_test_float_eq(R_even, 7.f);
    _gt_passed();
}

_gt_test(calc_median, empty_set_null_set) {
    // arrange
    const float set[0] = {};
    const unsigned size = 0;

    // act
    float R1 = calc_median(set, size);
    float R2 = calc_median(NULL, 2);

    // assert
    _gt_test_float_eq(R1, 0.f);
    _gt_test_float_eq(R2, 0.f);
    _gt_passed();
}

void calc_test(void) {
    _gt_run(calc_even, );
    _gt_run(calc_median, );
    _gt_run(calc_median, empty_set_null_set);
}