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

int gt_buf_write(gt_buf* buf, size_t size, void* data)
{
    GT_PRECOND(buf == NULL, GT_BUF_INVALID_BUF);
    GT_PRECOND(buf->data == NULL, GT_BUF_INVALID_BUF);
    GT_PRECOND(data == NULL, GT_BUF_INVALID_DATA);
    GT_PRECOND(size > buf->size, GT_BUF_FAILURE_SEGFAULT);

    memcpy(buf->data, data, size);
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

int gt_buf_distance(gt_buf* buf, gt_buf* seg, size_t* out_dist)
{
    GT_PRECOND(buf == NULL, GT_BUF_INVALID_BUF);
    GT_PRECOND(buf->data == NULL, GT_BUF_INVALID_BUF);
    GT_PRECOND(seg == NULL, GT_BUF_INVALID_SEG);
    GT_PRECOND(seg->data == NULL, GT_BUF_INVALID_SEG);
    GT_PRECOND(out_dist == NULL, GT_BUF_INVALID_OUT);

    uintptr_t bufptr = (uintptr_t)buf->data;
    uintptr_t segptr = (uintptr_t)seg->data;

    if (bufptr >= segptr && segptr < bufptr + seg->size) {
        *out_dist = segptr - bufptr;
        return GT_BUF_OK;
    }

    return gt_trace(GT_HERE, GT_BUF_FAILURE_SEGFAULT);
}
