/*
    Copyright (c) 2026 Gaël Fortier <gael.fortier.1@ens.etsmtl.ca>
*/

#include "../src/gt/lib.h"
#include "demo.h"

int called__get_vector_norm = 0;
number rtvl__get_vector_norm = 0.f;
number mock__get_vector_norm(vector v)
{
    called__get_vector_norm++;
    return rtvl__get_vector_norm;
}

gt_test(normalise_vector)
{
    gt_case(zero_norm)
    {
        // arrange
        _gt_set_hjck(normalise_vector);
        _gt_set_hook(mock__get_vector_norm);
        vector v = null_vector(VECTOR_TYPE_2D);
        rtvl__get_vector_norm = 0.f;

        // act
        vector nv = normalise_vector(v);

        // assert
        gt_assert_long(called__get_vector_norm, 1, GT_EQUALS_);
        gt_assert_long(nv.type, VECTOR_TYPE_INV, GT_EQUALS_);
    }
}

// entry point
int gt_test_main(int argc, char** argv)
{
    gt_run(normalise_vector);
    return 0;
}
