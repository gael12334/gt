/*
 * Copyright (c) 2026 Gaël Fortier <gael.fortier.1@ens.etsmtl.ca>
 */

#pragma once
#include "../buf/buf.h"
#include "../trace/trace.h"
#include <linux/limits.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
    GT_FILE_OK,
    GT_FILE_INVALID_PATH,
    GT_FILE_INVALID_OUT,
    GT_FILE_FAILURE_FOPEN,
    GT_FILE_FAILURE_MALLOC,
} gt_file_result;

int gt_file_unload(gt_buf* file);
int gt_file_save(gt_buf* buf, const char* path);
int gt_file_load(const char* path, gt_buf* out_file);
int gt_file_exists(const char* path, int* out_exists);
