/*
    Copyright © 2025 Gaël Fortier <gael.fortier.1@ens.etsmtl.ca>
*/

#include "elfpv_buffer.h"
#include <string.h>
#include <stdio.h>

static size_t size[] = {
    sizeof(int8_t),
    sizeof(int8_t),
    sizeof(int16_t),
    sizeof(int32_t),
    sizeof(int64_t),
    sizeof(uint8_t),
    sizeof(uint16_t),
    sizeof(uint32_t),
    sizeof(uint64_t),
    sizeof(size_t),
    sizeof(void*),
    0
};

static elfpv_buffer_error error = {0};
static int has_error = 0;

#define fails(condition) elfpv_fails(condition, #condition, __FILE__, __func__, __LINE__)

static int elfpv_fails(int condition, const char* expr, const char* file, const char* func, long line) {
    if(condition == 0) {
        error = (elfpv_buffer_error) {.expr = expr, .file = file, .func = func, .line = line};
        has_error = 1;
    }

    return !condition;
}

int elfpv_error_thrown(void) {
    return has_error;
}

elfpv_buffer_error elfpv_get_error(void) {
    return error;
}

elfpv_buffer elfpv_create_buffer(size_t size) {
    elfpv_buffer buffer = {0};
    buffer.buffer = malloc(size);
    if(fails(buffer.buffer == NULL)) { return buffer; }
    buffer.type = ELFPV_BUFFER_HEAP;
    buffer.size = size;
    has_error = 0;
    return buffer;
}

void elfpv_destroy_buffer(elfpv_buffer* buffer) {
    if(fails(buffer == NULL)) { return; }
    if(buffer->type == ELFPV_BUFFER_HEAP) { free(buffer->buffer); }
    if(buffer->type == ELFPV_BUFFER_STACKED ) { return; }
    *buffer = (elfpv_buffer) {0};
    has_error = 0;
}

elfpv_buffer elfpv_get_view(elfpv_buffer buffer, size_t offset, size_t size) {
    elfpv_buffer view = {0};
    if(fails(buffer.buffer == NULL)) { return view; }
    if(fails(offset >= buffer.size)) { return view; }
    if(fails(offset + size >= buffer.size)) { return view; }
    view.buffer = buffer.buffer;
    view.size = size;
    has_error = 0;
    return view;
}

elfpv_buffer_value elfpv_read_buffer(elfpv_buffer buffer, size_t offset, elfpv_buffer_value_type type) {
    elfpv_buffer_value value = {0};
    if(fails(buffer.buffer == NULL)) { return value; }
    if(fails(offset >= buffer.size)) { return value; }
    if(fails(type >= ELFPV_BUFFER_VALUE_COUNT)) { return value; }
    if(fails(offset + size[type] >= buffer.size)) { return value; }
    has_error = 0;
    switch(type) {
        case ELFPV_BUFFER_VALUE_INT8: return (elfpv_buffer_value) {._i8 = *((int8_t*)buffer.buffer + offset)};
        case ELFPV_BUFFER_VALUE_INT16: return (elfpv_buffer_value) {._i16 = *((int16_t*)buffer.buffer + offset)};
        case ELFPV_BUFFER_VALUE_INT32: return (elfpv_buffer_value) {._i32 = *((int32_t*)buffer.buffer + offset)};
        case ELFPV_BUFFER_VALUE_INT64: return (elfpv_buffer_value) {._i64 = *((int64_t*)buffer.buffer + offset)};
        case ELFPV_BUFFER_VALUE_UINT8: return (elfpv_buffer_value) {._u8 = *((uint8_t*)buffer.buffer + offset)};
        case ELFPV_BUFFER_VALUE_UINT16: return (elfpv_buffer_value) {._u16 = *((uint16_t*)buffer.buffer + offset)};
        case ELFPV_BUFFER_VALUE_UINT32: return (elfpv_buffer_value) {._u32 = *((uint32_t*)buffer.buffer + offset)};
        case ELFPV_BUFFER_VALUE_UINT64: return (elfpv_buffer_value) {._u64 = *((uint64_t*)buffer.buffer + offset)};
        case ELFPV_BUFFER_VALUE_SIZE: return (elfpv_buffer_value) {._z = *((size_t*)buffer.buffer + offset)};
        case ELFPV_BUFFER_VALUE_POINTER: return (elfpv_buffer_value) {._p = *((void**)buffer.buffer + offset)};
        default: return value;
    }
}

void elfpv_write_buffer(elfpv_buffer buffer, size_t offset, elfpv_buffer_value_type type, elfpv_buffer_value data) {
    printf("%i", type);

    elfpv_buffer_value value = {0};
    if(fails(buffer.buffer == NULL)) { return; }
    if(fails(offset >= buffer.size)) { return; }
    if(fails(type >= ELFPV_BUFFER_VALUE_COUNT)) { return; }
    if(fails(offset + size[type] >= buffer.size)) { return; }
    has_error = 0;
    memcpy(buffer.buffer + offset, &data, size[type]);
}
