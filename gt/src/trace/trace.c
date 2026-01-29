/*
 *  Copyright (c) 2026 Gaël Fortier <gael.fortier.1@ens.etsmtl.ca>
 */

#include "trace.h"
#define TRACE_STACK_SIZE (512)

struct {
    gt_trace_point stack[TRACE_STACK_SIZE];
    unsigned int   top;
} trace = {
    .stack = { 0 },
    .top = 0
};

int gt_trace(gt_location where, int result)
{
    if (result && trace.top < TRACE_STACK_SIZE) {
        gt_trace_point point = { .result = result, .location = where };
        trace.stack[trace.top] = point;
        trace.top++;
    }

    return result;
}

void gt_trace_reset(void)
{
    for (unsigned int i = 0; i < TRACE_STACK_SIZE; i++) {
        trace.stack[i] = (gt_trace_point) { 0 };
    }
    trace.top = 0;
}

int gt_trace_get_size(void)
{
    return trace.top;
}

int gt_trace_get_level(unsigned int level, gt_trace_point* out)
{
    if (level < trace.top) {
        *out = trace.stack[level];
        return 0;
    }

    return 1;
}

int gt_trace_get_result(void)
{
    return (trace.top) ? trace.stack[trace.top - 1].result : 0;
}

int gt_trace_foward(gt_location where)
{
    int r = gt_trace_get_result();
    return gt_trace(where, r);
}
