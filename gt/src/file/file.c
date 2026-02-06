/*
 * Copyright (c) 2026 Gaël Fortier <gael.fortier.1@ens.etsmtl.ca>
 */

#include "file.h"
#include "../heap/heap.h"
#include "../trace/trace.h"

int gt_file_load(const char* path, gt_buf* out_buf)
{
    GT_THROWIF(path == NULL, GT_FILE_INVALID_PATH);
    GT_THROWIF(out_buf == NULL, GT_FILE_INVALID_OUT);

    FILE* file = fopen(path, "rb");
    GT_THROWIF(file == NULL, GT_FILE_FAILURE_FOPEN);

    fseek(file, 0, SEEK_END);
    size_t size = ftell(file);
    rewind(file);

    void* data = NULL;
    GT_TRYSAFE(gt_heap_allocate(size, &data), {
        fclose(file);
    });

    fread(data, 1, size, file);
    fclose(file);

    GT_TRYSAFE(gt_buf_init(size, data, out_buf), {
        free(data);
    });

    return GT_FILE_OK;
}

int gt_file_unload(gt_buf* buf)
{
    void* data = NULL;
    GT_TRYTHIS(gt_buf_data(buf, &data));
    GT_TRYTHIS(gt_heap_liberate(data));
    GT_TRYTHIS(gt_buf_deinit(buf));
    return GT_FILE_OK;
}

int gt_file_save(gt_buf* buf, const char* path)
{
    GT_THROWIF(path == NULL, GT_FILE_INVALID_PATH);

    void* data = NULL;
    FILE* file = fopen(path, "wb");
    GT_THROWIF(file == NULL, GT_FILE_FAILURE_FOPEN);
    GT_TRYSAFE(gt_buf_data(buf, &data), {
        fclose(file);
    });

    fwrite(data, 1, buf->size, file);
    fclose(file);
    return GT_FILE_OK;
}

int gt_file_exists(const char* path, int* out_exists)
{
    GT_THROWIF(path == NULL, GT_FILE_INVALID_PATH);
    GT_THROWIF(out_exists == NULL, GT_FILE_INVALID_OUT);

    FILE* file = fopen(path, "r");
    *out_exists = (file != NULL);

    if (*out_exists)
        fclose(file);

    return GT_FILE_OK;
}
