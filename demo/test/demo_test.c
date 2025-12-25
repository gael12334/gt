/*
    Copyright © 2025 Gaël Fortier <gael.fortier.1@ens.etsmtl.ca>
*/

#include "../../pkg/include/gt1.h"
#include "../src/demo.h"

static int* mock_received = NULL;
static int mock_called = 0;
int accumulate_hook(struct int_list list) {
    mock_received = list.intlist;
    mock_called = 1;
    return 100;
}

gt_test(accumulate) {
    gt_case("test") {
        // arrange
        _gt_set_hjck(accumulate);
        _gt_set_hook(accumulate_hook);
        int arr[] = {0, 1, 2, 3, 4};
        int sz = 5;
        struct int_list list = { .intlist = arr, .length = sz };

        // act
        int r = average(list);

        // assert
        gt_assert_long(r, 20, GT_EQUALS_);
    }
}

int main(int argc, char** argv) {
    gt_run(accumulate);
    return 0;
}
