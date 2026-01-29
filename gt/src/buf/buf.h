/*
 *  Copyright (c) 2026 Gaël Fortier <gael.fortier.1@ens.etsmtl.ca>
 */

#pragma once
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
    GT_BUF_OK,
    GT_BUF_INVALID_DATA,
    GT_BUF_INVALID_SIZE,
    GT_BUF_INVALID_OUT,
    GT_BUF_INVALID_OFFSET,
    GT_BUF_INVALID_BUF,
    GT_BUF_INVALID_SEG,
    GT_BUF_FAILURE_SEGFAULT,
    GT_BUF_FAILURE_UNZEROED,
} gt_buf_result;

typedef struct {
    size_t size;
    void*  data;
} gt_buf;

int gt_buf_init(size_t size, void* data, gt_buf* out_buf);
int gt_buf_deinit(gt_buf* buf);
int gt_buf_segment(gt_buf* buf, size_t offset, size_t size, gt_buf* out_seg);
int gt_buf_write(gt_buf* buf, size_t size, void* data);
int gt_buf_read(gt_buf* buf, size_t size, void* data);
int gt_buf_distance(gt_buf* buf, gt_buf* seg, size_t* out_dist);
