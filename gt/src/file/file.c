/*
 * Copyright (c) 2025 Gaël Fortier <gael.fortier.1@ens.etsmtl.ca>
 */

#include "file.h"
#include "../trace/trace.h"

int gt_file_load(const char* path, gt_file* out_file)
{
    FILE*  file;
    size_t size;
    void*  address;

    if (path == NULL || out_file == NULL) {
        return gt_trace(GT_FILE_NULL, GT_HERE);
    } else if (out_file->bytes != NULL || out_file->size != 0) {
        return gt_trace(GT_FILE_LOADED, GT_HERE);
    } else if ((file = fopen(path, "rb")) == NULL) {
        return gt_trace(GT_FILE_INVALID, GT_HERE);
    }

    fseek(file, 0, SEEK_END);
    size = ftell(file);
    rewind(file);

    if ((address = malloc(out_file->size)) == NULL) {
        fclose(file);
        return gt_trace(GT_FILE_MALLOC, GT_HERE);
    }

    fread(address, 1, size, file);
    fclose(file);

    out_file->bytes = address;
    out_file->size = size;
    memset(out_file->path, 0, sizeof(out_file->path));
    strncpy(out_file->path, path, sizeof(out_file->path) - 1);

    return GT_FILE_OK;
}

int gt_file_unload(gt_file* file)
{
    if (file == NULL) {
        return gt_trace(GT_FILE_NULL, GT_HERE);
    } else if (file->bytes == NULL) {
        return gt_trace(GT_FILE_UNLOADED, GT_HERE);
    }

    gt_file temp = *file;
    memset(file, 0, sizeof(*file));
    memset(temp.bytes, 0, temp.size);
    free(temp.bytes);

    return GT_FILE_OK;
}

int gt_file_save(const char* path, gt_file* file)
{
    FILE* fd;

    if (path == NULL || file == NULL) {
        return gt_trace(GT_FILE_NULL, GT_HERE);
    } else if ((fd = fopen(path, "wb")) == NULL) {
        return gt_trace(GT_FILE_INVALID, GT_HERE);
    }

    fwrite(file->bytes, 1, file->size, fd);
    fclose(fd);
    return GT_FILE_OK;
}

int gt_file_getaddr(gt_file* file, size_t offset, void** out_addr)
{
    if (file == NULL || out_addr == NULL) {
        return gt_trace(GT_FILE_NULL, GT_HERE);
    } else if (file->bytes == NULL) {
        return gt_trace(GT_FILE_UNLOADED, GT_HERE);
    } else if (offset >= file->size) {
        return gt_trace(GT_FILE_OFFSET, GT_HERE);
    }

    *out_addr = file->bytes + offset;
    return GT_FILE_OK;
}

int gt_file_writeat(gt_file* file, size_t offset, void* data, size_t size)
{
    void* start_fileaddr;
    void* xend_fileaddr;
    int   error;

    if ((error = gt_file_getrange(file, offset, size, &start_fileaddr, &xend_fileaddr))) {
        return gt_trace(error, GT_HERE);
    }

    memcpy(start_fileaddr, data, size);
    return GT_FILE_OK;
}

int gt_file_getrange(gt_file* file, size_t offset, size_t size, void** out_start, void** out_xend)
{
    int error;

    if ((error = gt_file_getaddr(file, offset, out_start))) {
        return gt_trace(error, GT_HERE);
    } else if (size == 0) {
        *out_xend = (*out_start) + 1;
        return GT_FILE_OK;
    }

    if ((error = gt_file_getaddr(file, offset + size - 1, out_xend))) {
        return gt_trace(error, GT_HERE);
    }

    (*out_xend)++;
    return GT_FILE_OK;
}
