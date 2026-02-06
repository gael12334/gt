/*
 *  Copyright (c) 2026 Gaël Fortier <gael.fortier.1@ens.etsmtl.ca>
 */

#include "heap.h"
#include <stdio.h>

static void* allocd_ptr = NULL;

static void test_gt_heap_allocate(void)
{
    // arrange
    size_t size = 10;

    // act
    int result = gt_heap_allocate(size, &allocd_ptr);

    // assert
    if (result)
        printf("result was %i\n", result);
    else if (allocd_ptr == NULL)
        printf("ptr was NULL\n");
    else
        printf("ok\n");
}

static void test_gt_heap_relocate(void)
{
    // arrange
    size_t new_size = 20;
    void*  old_ptr = allocd_ptr;
    allocd_ptr = NULL;

    // act
    int result = gt_heap_relocate(new_size, old_ptr, &allocd_ptr);

    // assert
    if (result)
        printf("result was %i\n", result);
    else if (allocd_ptr == NULL)
        printf("ptr was NULL\n");
    else
        printf("ok\n");
}

static void test_gt_heap_liberate(void)
{
    // arrange

    // act
    int result = gt_heap_liberate(allocd_ptr);

    // assert
    if (result)
        printf("result was %i\n", result);
    else
        printf("ok\n");
}

int main(int argc, char** argv)
{
    test_gt_heap_allocate();
    if (allocd_ptr == NULL)
        return 0;

    test_gt_heap_relocate();
    if (allocd_ptr == NULL)
        return 0;

    test_gt_heap_liberate();
    return 0;
}
