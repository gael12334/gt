/*
 *  Copyright (c) 2026 Gaël Fortier <gael.fortier.1@ens.etsmtl.ca>
 */

#include "buf.h"
#include "../trace/trace.h"
#include <string.h>

int gt_buf_init(size_t size, void* data, gt_buf* out_buf)
{
    GT_PRECOND(data == NULL, GT_BUF_INVALID_DATA);
    GT_PRECOND(out_buf == NULL, GT_BUF_INVALID_OUT);
    GT_PRECOND(out_buf->data != NULL, GT_BUF_FAILURE_UNZEROED);
    GT_PRECOND(out_buf->size != 0, GT_BUF_FAILURE_UNZEROED);
    out_buf->data = data;
    out_buf->size = size;
    return GT_BUF_OK;
}

int gt_buf_deinit(gt_buf* buf)
{
    GT_PRECOND(buf == NULL, GT_BUF_INVALID_BUF);
    memset(buf, 0, sizeof(*buf));
    return GT_BUF_OK;
}

int gt_buf_segment(gt_buf* buf, size_t offset, size_t size, gt_buf* out_seg)
{
    GT_PRECOND(buf == NULL, GT_BUF_INVALID_BUF);
    GT_PRECOND(buf->data == NULL, GT_BUF_INVALID_BUF);
    GT_PRECOND(offset + size > buf->size, GT_BUF_FAILURE_SEGFAULT);

    if (gt_buf_init(size, buf->data + offset, out_seg))
        return gt_trace_foward(GT_HERE);

    return GT_BUF_OK;
}

int gt_buf_write(gt_buf* buf, size_t size, const void* data)
{
    GT_PRECOND(buf == NULL, GT_BUF_INVALID_BUF);
    GT_PRECOND(buf->data == NULL, GT_BUF_INVALID_BUF);
    GT_PRECOND(data == NULL, GT_BUF_INVALID_DATA);
    GT_PRECOND(size > buf->size, GT_BUF_FAILURE_SEGFAULT);

    memcpy(buf->data, data, size);
    return GT_BUF_OK;
}

int gt_buf_writeat(gt_buf* buf, size_t offset, size_t size, const void* data)
{
    gt_buf seg = { 0 };
    if (gt_buf_segment(buf, offset, size, &seg))
        return gt_trace_foward(GT_HERE);
    if (gt_buf_write(&seg, size, data))
        return gt_trace_foward(GT_HERE);
    return GT_BUF_OK;
}

int gt_buf_read(gt_buf* buf, size_t size, void* data)
{
    GT_PRECOND(buf == NULL, GT_BUF_INVALID_BUF);
    GT_PRECOND(buf->data == NULL, GT_BUF_INVALID_BUF);
    GT_PRECOND(data == NULL, GT_BUF_INVALID_DATA);
    GT_PRECOND(size < buf->size, GT_BUF_INVALID_SIZE);

    memcpy(data, buf->data, size);
    return GT_BUF_OK;
}

int gt_buf_readat(gt_buf* buf, size_t offset, size_t size, void* data)
{
    gt_buf seg = { 0 };
    if (gt_buf_segment(buf, offset, size, &seg))
        return gt_trace_foward(GT_HERE);
    if (gt_buf_read(&seg, size, data))
        return gt_trace_foward(GT_HERE);
    return GT_BUF_OK;
}

// Distance from the start of a buffer to the start of a segment of itself.
// If start of segment outside buffer's range, returns GT_BUF_FAILURE_SEGFAULT.
int gt_buf_distance(gt_buf* buf, gt_buf* seg, size_t* out_dist)
{
    GT_PRECOND(buf == NULL, GT_BUF_INVALID_BUF);
    GT_PRECOND(buf->data == NULL, GT_BUF_INVALID_BUF);
    GT_PRECOND(seg == NULL, GT_BUF_INVALID_SEG);
    GT_PRECOND(seg->data == NULL, GT_BUF_INVALID_SEG);
    GT_PRECOND(out_dist == NULL, GT_BUF_INVALID_OUT);

    uintptr_t bufdata = (uintptr_t)buf->data;
    uintptr_t segdata = (uintptr_t)seg->data;

    GT_PRECOND(segdata < bufdata, GT_BUF_FAILURE_SEGFAULT);
    GT_PRECOND(segdata >= bufdata + buf->size, GT_BUF_FAILURE_SEGFAULT);

    *out_dist = seg->data - buf->data;
    return GT_BUF_OK;
}

int gt_buf_data(gt_buf* buf, void** out_data)
{
    GT_PRECOND(buf == NULL, GT_BUF_INVALID_BUF);
    GT_PRECOND(buf->data == NULL, GT_BUF_INVALID_BUF);
    GT_PRECOND(out_data == NULL, GT_BUF_INVALID_OUT);
    GT_PRECOND(*out_data != NULL, GT_BUF_INVALID_OUT);

    *out_data = buf->data;
    return GT_BUF_OK;
}
