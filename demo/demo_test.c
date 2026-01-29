/*
    Copyright (c) 2026 Gaël Fortier <gael.fortier.1@ens.etsmtl.ca>
*/

#include "../src/gt2.h"
#include "demo.h"

GT_STRUCT_OF(normalise_vector)
{
    vector retval;
    vector provided;
    int called;
};

vector GT_MOCK_OF(normalise_vector)(vector vector_1)
{
    GT_STRUCT_OF(normalise_vector)* data = gt_get_data(normalise_vector);
    data->provided = vector_1;
    data->called++;
    return data->retval;
}

// entry point
int gt_test_main()
{
    gt_test_function(normalise_vector)
    {
        gt_test_case(null_vector)
        {
            // arrange
            typedef GT_STRUCT_OF(normalise_vector) data_t;
            data_t data = {
                .called = 0,
                .retval = (vector) {
                    .x = 0.f,
                    .y = 0.f,
                    .z = 0.f,
                    .w = 0.f,
                }
            };

            gt_set_mock(normalise_vector, &data);
            vector v = { .type = VECTOR_TYPE_1D };

            // act
            vector r = normalise_vector(v);

            // assert
            gt_assert(r.type, GT_MODE_EQUAL, VECTOR_TYPE_3D);
            gt_assert(r.x, GT_MODE_EQUAL, 0.f);
        }
    }

    return 0;
}
