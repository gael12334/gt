/*
    Copyright © 2025 Gaël Fortier <gael.fortier.1@ens.etsmtl.ca>
*/

#pragma  once

#include <stddef.h>
#include <stdlib.h>
#include <stdint.h>

#define ELFPV_BUFFER_STACKED 0b00000001
#define ELFPV_BUFFER_VIEW    0b00000010
#define ELFPV_BUFFER_HEAP    0b00000100

typedef struct {
    void* buffer;
    size_t size;
    int type;
} elfpv_buffer;

typedef struct {
    const char* expr;
    const char* file;
    const char* func;
    long line;
} elfpv_buffer_error;

typedef union {
    int8_t _i8;
    uint8_t _u8;
    int16_t _i16;
    uint16_t _u16;
    int32_t _i32;
    uint32_t _u32;
    int64_t _i64;
    uint64_t _u64;
    size_t _z;
    void* _p;
} elfpv_buffer_value;

typedef enum {
    ELFPV_BUFFER_VALUE_INT8,
    ELFPV_BUFFER_VALUE_INT16,
    ELFPV_BUFFER_VALUE_INT32,
    ELFPV_BUFFER_VALUE_INT64,
    ELFPV_BUFFER_VALUE_UINT8,
    ELFPV_BUFFER_VALUE_UINT16,
    ELFPV_BUFFER_VALUE_UINT32,
    ELFPV_BUFFER_VALUE_UINT64,
    ELFPV_BUFFER_VALUE_SIZE,
    ELFPV_BUFFER_VALUE_POINTER,
    ELFPV_BUFFER_VALUE_COUNT
} elfpv_buffer_value_type;

int elfpv_error_thrown(void);
elfpv_buffer_error elfpv_get_error(void);
elfpv_buffer elfpv_create_buffer(size_t size);
void elfpv_destroy_buffer(elfpv_buffer* buffer);
elfpv_buffer elfpv_get_view(elfpv_buffer buffer, size_t offset, size_t size);
elfpv_buffer_value elfpv_read_buffer(elfpv_buffer buffer, size_t offset, elfpv_buffer_value_type type);
void elfpv_write_buffer(elfpv_buffer buffer, size_t offset, elfpv_buffer_value_type type, elfpv_buffer_value data);

#define elfpv_print_buffer_error() { elfpv_buffer_error e = elfpv_get_error(); printf("%s %s %s %li\n", e.expr, e.file, e.func, e.line); }
