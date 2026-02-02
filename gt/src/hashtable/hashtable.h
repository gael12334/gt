/*
 *  Copyright (c) 2026 Gaël Fortier <gael.fortier.1@ens.etsmtl.ca>
 */

#pragma once
#include "../buf/buf.h"

typedef struct {
    void*         caller;
    size_t        size;
    const void*   data;
    unsigned int* out;
} gt_hfparams;

typedef int (*gt_hashfunc)(gt_hfparams* params);

typedef struct {
    gt_buf      buf;
    gt_hashfunc hashfunc;
} gt_hashtable;
