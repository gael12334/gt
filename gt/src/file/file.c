/*
 * Copyright (c) 2026 Gaël Fortier <gael.fortier.1@ens.etsmtl.ca>
 */

#include "file.h"
#include "../trace/trace.h"

int gt_file_load(const char* path, gt_buf* out_buf)
{
    GT_PRECOND(path == NULL, GT_FILE_INVALID_PATH);
    GT_PRECOND(out_buf == NULL, GT_FILE_INVALID_OUT);

    FILE* file = fopen(path, "rb");
    if (file == NULL)
        return gt_trace(GT_HERE, GT_FILE_FAILURE_FOPEN);

    fseek(file, 0, SEEK_END);
    size_t size = ftell(file);
    rewind(file);

    void* data = malloc(size);
    if (data == NULL) {
        fclose(file);
        return gt_trace(GT_HERE, GT_FILE_FAILURE_MALLOC);
    }

    fread(data, 1, size, file);
    fclose(file);

    if (gt_buf_init(size, data, out_buf)) {
        free(data);
        return gt_trace_foward(GT_HERE);
    }

    return GT_FILE_OK;
}

int gt_file_unload(gt_buf* buf)
{
    void* data = NULL;
    if (gt_buf_data(buf, &data))
        return gt_trace_foward(GT_HERE);

    free(data);
    if (gt_buf_deinit(buf))
        return gt_trace_foward(GT_HERE);

    return GT_FILE_OK;
}

int gt_file_save(gt_buf* buf, const char* path)
{
    GT_PRECOND(path == NULL, GT_FILE_INVALID_PATH);

    FILE* file = fopen(path, "wb");
    if (file == NULL)
        return gt_trace(GT_HERE, GT_FILE_FAILURE_FOPEN);

    void* data = NULL;
    if (gt_buf_data(buf, &data)) {
        fclose(file);
        return gt_trace_foward(GT_HERE);
    }

    fwrite(data, 1, buf->size, file);
    fclose(file);
    return GT_FILE_OK;
}

int gt_file_exists(const char* path, int* out_exists)
{
    GT_PRECOND(path == NULL, GT_FILE_INVALID_PATH);
    GT_PRECOND(out_exists == NULL, GT_FILE_INVALID_OUT);

    FILE* file = fopen(path, "r");
    if (file == NULL) {
        *out_exists = 0;
        return GT_FILE_OK;
    }

    *out_exists = 1;
    fclose(file);
    return GT_FILE_OK;
}
