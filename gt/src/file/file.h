/*
 * Copyright (c) 2026 Gaël Fortier <gael.fortier.1@ens.etsmtl.ca>
 */

#pragma once
#include "../trace/trace.h"
#include <linux/limits.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
    GT_FILE_OK,
    GT_FILE_NULL,
    GT_FILE_LOADED,
    GT_FILE_UNLOADED,
    GT_FILE_INVALID,
    GT_FILE_MALLOC,
    GT_FILE_OFFSET
} gt_file_result;

typedef struct {
    char     path[PATH_MAX];
    uint8_t* bytes;
    size_t   size;
} gt_file;

int gt_file_unload(gt_file* file);
int gt_file_save(const char* path, gt_file* file);
int gt_file_load(const char* path, gt_file* out_file);
int gt_file_getaddr(gt_file* file, size_t offset, void** out_addr);
int gt_file_writeat(gt_file* file, size_t offset, void* data, size_t size);
int gt_file_getrange(gt_file* file, size_t offset, size_t size, void** out_start, void** out_excl_end);
