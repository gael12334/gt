/*
 *  Copyright (c) 2026 Gaël Fortier <gael.fortier.1@ens.etsmtl.ca>
 */

#include "buf.h"
#include "../trace/trace.h"
#include <stdint.h>
#include <string.h>

int gt_buf_init(size_t size, void* data, gt_buf* out_buf)
{
    GT_THROWIF(data == NULL, GT_BUF_INVALID_DATA);
    GT_THROWIF(out_buf == NULL, GT_BUF_INVALID_OUT);
    GT_THROWIF(out_buf->data != NULL, GT_BUF_FAILURE_UNZEROED);
    GT_THROWIF(out_buf->size != 0, GT_BUF_FAILURE_UNZEROED);
    out_buf->data = data;
    out_buf->size = size;
    return GT_BUF_OK;
}

int gt_buf_deinit(gt_buf* buf)
{
    GT_THROWIF(buf == NULL, GT_BUF_INVALID_BUF);
    memset(buf, 0, sizeof(*buf));
    return GT_BUF_OK;
}

int gt_buf_segment(gt_buf* buf, size_t offset, size_t size, gt_buf* out_seg)
{
    GT_THROWIF(buf == NULL, GT_BUF_INVALID_BUF);
    GT_THROWIF(buf->data == NULL, GT_BUF_INVALID_BUF);
    GT_THROWIF(offset + size > buf->size, GT_BUF_FAILURE_SEGFAULT);

    if (gt_buf_init(size, buf->data + offset, out_seg))
        return gt_trace_foward(GT_HERE);

    return GT_BUF_OK;
}

int gt_buf_effective(gt_buf* buf, size_t offset, size_t size, void** out)
{
    gt_buf seg = GT_BUF_ZEROED;
    GT_TRYTHIS(gt_buf_segment(buf, offset, size, &seg));
    GT_TRYTHIS(gt_buf_data(&seg, out));
    return GT_BUF_OK;
}

int gt_buf_write(gt_buf* buf, size_t size, const void* data)
{
    GT_THROWIF(buf == NULL, GT_BUF_INVALID_BUF);
    GT_THROWIF(buf->data == NULL, GT_BUF_INVALID_BUF);
    GT_THROWIF(data == NULL, GT_BUF_INVALID_DATA);
    GT_THROWIF(size > buf->size, GT_BUF_FAILURE_SEGFAULT);

    memcpy(buf->data, data, size);
    return GT_BUF_OK;
}

int gt_buf_writeat(gt_buf* buf, size_t offset, size_t size, const void* data)
{
    gt_buf seg = GT_BUF_ZEROED;
    if (gt_buf_segment(buf, offset, size, &seg))
        return gt_trace_foward(GT_HERE);
    if (gt_buf_write(&seg, size, data))
        return gt_trace_foward(GT_HERE);
    return GT_BUF_OK;
}

int gt_buf_read(gt_buf* buf, size_t size, void* data)
{
    GT_THROWIF(buf == NULL, GT_BUF_INVALID_BUF);
    GT_THROWIF(buf->data == NULL, GT_BUF_INVALID_BUF);
    GT_THROWIF(data == NULL, GT_BUF_INVALID_DATA);
    GT_THROWIF(size < buf->size, GT_BUF_INVALID_SIZE);

    memcpy(data, buf->data, size);
    return GT_BUF_OK;
}

int gt_buf_readat(gt_buf* buf, size_t offset, size_t size, void* data)
{
    gt_buf seg = GT_BUF_ZEROED;
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
    GT_THROWIF(buf == NULL, GT_BUF_INVALID_BUF);
    GT_THROWIF(buf->data == NULL, GT_BUF_INVALID_BUF);
    GT_THROWIF(seg == NULL, GT_BUF_INVALID_SEG);
    GT_THROWIF(seg->data == NULL, GT_BUF_INVALID_SEG);
    GT_THROWIF(out_dist == NULL, GT_BUF_INVALID_OUT);

    uintptr_t bufdata = (uintptr_t)buf->data;
    uintptr_t segdata = (uintptr_t)seg->data;

    GT_THROWIF(segdata < bufdata, GT_BUF_FAILURE_SEGFAULT);
    GT_THROWIF(segdata >= bufdata + buf->size, GT_BUF_FAILURE_SEGFAULT);

    *out_dist = seg->data - buf->data;
    return GT_BUF_OK;
}

int gt_buf_data(gt_buf* buf, void** out_data)
{
    GT_THROWIF(buf == NULL, GT_BUF_INVALID_BUF);
    GT_THROWIF(buf->data == NULL, GT_BUF_INVALID_BUF);
    GT_THROWIF(out_data == NULL, GT_BUF_INVALID_OUT);
    GT_THROWIF(*out_data != NULL, GT_BUF_INVALID_OUT);

    *out_data = buf->data;
    return GT_BUF_OK;
}

int gt_buf_size(gt_buf* buf, size_t* out_size)
{
    GT_THROWIF(buf == NULL, GT_BUF_INVALID_BUF);
    GT_THROWIF(buf->data == NULL, GT_BUF_INVALID_BUF);
    GT_THROWIF(out_size == NULL, GT_BUF_INVALID_OUT);

    *out_size = buf->size;
    return GT_BUF_OK;
}
#include <stdio.h>
int gt_buf_equals(gt_buf* buf1, gt_buf* buf2, int* out)
{
    GT_THROWIF(buf1 == NULL, GT_BUF_INVALID_BUF);
    GT_THROWIF(buf1->data == NULL, GT_BUF_INVALID_BUF);
    GT_THROWIF(out == NULL, GT_BUF_INVALID_OUT);

    uint64_t  index = 0;
    uint64_t  equals = 1;
    uint64_t* bucket1 = buf1->data;
    uint64_t* bucket2 = buf2->data;

    equals = (buf1->size == buf2->size);
    while (equals && index < buf1->size) {
        uint64_t next = index + sizeof(uint64_t);
        uint64_t over = (next - buf1->size) * (next > buf1->size);
        uint64_t mask = UINT64_MAX >> (over * 8);
        uint64_t val1 = *bucket1 & mask;
        uint64_t val2 = *bucket2 & mask;
        equals &= (val1 == val2);
        index = next - over;
        bucket1++;
        bucket2++;
    }

    *out = equals;
    return GT_BUF_OK;
}
