/*
 * Copyright (c) 2025 Gaël Fortier <gael.fortier.1@ens.etsmtl.ca>
 */

#include "trace.h"
#include <stdio.h>

void test_gt_trace(void)
{
    // arrange
    int         result = 1;
    gt_location here = GT_HERE;

    // act
    int r = gt_trace(here, result);

    // assert
    int size;
    if ((size = gt_trace_get_size()) != 1)
        printf("stack top is %i\n", size);
    else
        printf("ok\n");
}

void test_gt_trace_reset(void)
{
    // arrange
    gt_trace(GT_HERE, 1);

    // act
    gt_trace_reset();

    // assert
    int size;
    if ((size = gt_trace_get_size()) != 0)
        printf("stack top is %i\n", size);
    else
        printf("ok\n");
}

void test_gt_trace_get_level(void)
{
    // arrange
    gt_location here = GT_HERE;
    gt_trace(here, 1);

    // act
    int            result;
    gt_trace_point point;
    result = gt_trace_get_level(0, &point);

    // assert
    const char* file = point.location.file;
    int         line = point.location.line;

    if (result) {
        printf("returned %i\n", result);
        return;
    }

    if (file != here.file && line != here.line) {
        printf("location was wrong: %s @ %i\n", file, line);
        return;
    }

    printf("ok\n");
    gt_trace_reset();
}

void test_gt_trace_get_result(void)
{
    // arrange
    gt_trace(GT_HERE, 2);

    // act
    int result = gt_trace_get_result();

    // assert
    if (result != 2)
        printf("expected 2, got %i\n", result);
    else
        printf("ok\n");

    gt_trace_reset();
}

void test_gt_trace_foward(void)
{
    // arrange
    gt_trace(GT_HERE, 2);

    // act
    int result = gt_trace_foward(GT_HERE);

    // assert
    if (result != 2)
        printf("expected 2, got %i\n", result);
    else
        printf("ok\n");

    gt_trace_reset();
}

int main(int argc, char** argv)
{
    test_gt_trace();
    test_gt_trace_reset();
    test_gt_trace_get_level();
    test_gt_trace_get_result();
    test_gt_trace_foward();
    return 0;
}
