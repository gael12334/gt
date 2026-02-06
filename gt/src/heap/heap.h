/*
 *  Copyright (c) 2026 Gaël Fortier <gael.fortier.1@ens.etsmtl.ca>
 */

#pragma once
#include "../trace/trace.h"
#include <stdlib.h>

typedef enum {
    GT_HEAP_OK,
    GT_HEAP_INVALID_OUT,
    GT_HEAP_INVALID_PTR,
    GT_HEAP_INVALID_OLD,
    GT_HEAP_FAILURE_ALLOC,
} gt_heap_result;

int gt_heap_allocate(size_t size, void** out);
int gt_heap_liberate(void* ptr);
int gt_heap_relocate(size_t size, void* old, void** out);
