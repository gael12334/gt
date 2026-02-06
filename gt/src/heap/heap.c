/*
 *  Copyright (c) 2026 Gaël Fortier <gael.fortier.1@ens.etsmtl.ca>
 */

#include "heap.h"

int gt_heap_allocate(size_t size, void** out)
{
    GT_THROWIF(out == NULL, GT_HEAP_INVALID_OUT);
    GT_THROWIF(*out != NULL, GT_HEAP_INVALID_OUT);
    void* ptr = malloc(size);
    GT_THROWIF(ptr == NULL, GT_HEAP_FAILURE_ALLOC);
    *out = ptr;
    return GT_HEAP_OK;
}

int gt_heap_liberate(void* ptr)
{
    GT_THROWIF(ptr == NULL, GT_HEAP_INVALID_PTR);
    free(ptr);
    return GT_HEAP_OK;
}

int gt_heap_relocate(size_t size, void* old, void** out)
{
    GT_THROWIF(out == NULL, GT_HEAP_INVALID_OUT);
    GT_THROWIF(*out != NULL, GT_HEAP_INVALID_OUT);
    void* ptr = realloc(old, size);
    GT_THROWIF(ptr == NULL, GT_HEAP_FAILURE_ALLOC);
    *out = ptr;
    return GT_HEAP_OK;
}
