/*
 *  Copyright (c) 2026 Gaël Fortier <gael.fortier.1@ens.etsmtl.ca>
 */

#pragma once
#include "../location/location.h"

#define GT_PRECOND(condition, result)     \
    if (condition) {                      \
        return gt_trace(GT_HERE, result); \
    }

typedef struct {
    int         result;
    gt_location location;
} gt_trace_point;

int gt_trace(gt_location where, int result);

void gt_trace_reset(void);

int gt_trace_get_size(void);

int gt_trace_get_level(unsigned int level, gt_trace_point* out);

int gt_trace_get_result(void);

int gt_trace_foward(gt_location where);
