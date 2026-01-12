/*
    Copyright (c) 2026 Gaël Fortier <gael.fortier.1@ens.etsmtl.ca>
*/

#pragma once
#include <stdint.h>

typedef union {
    uint64_t u_64;
    int64_t s_64;
    double f_64;
    const char* cstr;
    const void* addr;
} gt_generic;

gt_generic gt_explicit_genericof_u_64(uint64_t u_64);
gt_generic gt_explicit_genericof_s_64(int64_t s_64);
gt_generic gt_explicit_genericof_f_64(double f_64);
gt_generic gt_explicit_genericof_cstr(const char* str);
gt_generic gt_explicit_genericof_vptr(const void* ptr);
#define gt_genericof(x)                       \
    _Generic((x),                             \
        uint8_t: gt_explicit_genericof_u_64,  \
        uint16_t: gt_explicit_genericof_u_64, \
        uint32_t: gt_explicit_genericof_u_64, \
        uint64_t: gt_explicit_genericof_u_64, \
        int8_t: gt_explicit_genericof_s_64,   \
        int16_t: gt_explicit_genericof_s_64,  \
        int32_t: gt_explicit_genericof_s_64,  \
        int64_t: gt_explicit_genericof_s_64,  \
        float: gt_explicit_genericof_f_64,    \
        double: gt_explicit_genericof_f_64,   \
        char*: gt_explicit_genericof_cstr,    \
        void*: gt_explicit_genericof_vptr)(x)

typedef enum {
    GT_TYPE_U_64,
    GT_TYPE_S_64,
    GT_TYPE_F_64,
    GT_TYPE_CSTR,
    GT_TYPE_ADDR,
    GT_TYPE_NMBR
} gt_type;
#define gt_typeof(x)            \
    _Generic((x),               \
        uint8_t: GT_TYPE_U_64,  \
        uint16_t: GT_TYPE_U_64, \
        uint32_t: GT_TYPE_U_64, \
        uint64_t: GT_TYPE_U_64, \
        int8_t: GT_TYPE_S_64,   \
        int16_t: GT_TYPE_S_64,  \
        int32_t: GT_TYPE_S_64,  \
        int64_t: GT_TYPE_S_64,  \
        float: GT_TYPE_F_64,    \
        double: GT_TYPE_F_64,   \
        char*: GT_TYPE_CSTR,    \
        void*: GT_TYPE_ADDR)

typedef enum {
    GT_MODE_EQUAL,
    GT_MODE_NT_EQ,
    GT_MODE_LESSR,
    GT_MODE_BIGGR,
    GT_MODE_LE_EQ,
    GT_MODE_GR_EQ,
    GT_MODE_NUMBR
} gt_mode;

typedef struct {
    const char* expression;
    gt_generic generic;
    gt_type type;
} gt_expression;

enum {
    GT_LOOP_ENTER,
    GT_LOOP_LEAVE,
};

int gt_explicit_test_function(const char* name);
#define gt_test_function(function) \
    while (gt_explicit_test_function(#function) == GT_LOOP_ENTER)

int gt_explicit_test_case(const char* name);
#define gt_test_case(name) \
    while (gt_explicit_test_case(#name) == GT_LOOP_ENTER)

int gt_explicit_assert(gt_expression a, gt_mode mode, gt_expression b, const char* file, unsigned line);
#define gt_assert(a, mode, b)                                                                     \
    {                                                                                             \
        gt_expression x = { .expression = #a, .generic = gt_genericof(a), .type = gt_typeof(a) }; \
        gt_expression y = { .expression = #b, .generic = gt_genericof(b), .type = gt_typeof(b) }; \
        if (gt_explicit_assert(x, mode, y, __FILE__, __LINE__) == GT_LOOP_LEAVE) {                \
            continue;                                                                             \
        }                                                                                         \
    }

void gt_explicit_set_mock(void* function_ptr, void* mock_ptr, void* data);
#define GT_MOCK_OF(function) gt_mockof_##function
#define gt_set_mock(function, data) \
    gt_explicit_set_mock(function, GT_MOCK_OF(function), data)

int gt_explicit_unmocked_section(void** mock);
#define gt_unmocked_section(mock) \
    while (gt_explicit_unmocked_section(mock) == GT_LOOP_ENTER)

void* gt_explicit_get_data(void);
#define GT_STRUCT_OF(function) struct gt_structof_##function
#define gt_get_data(function) \
    (GT_STRUCT_OF(function)*)gt_explicit_get_data()

const char* gt_function_hijack_symbol_name(void);
const char* gt_function_mock_symbol_name(void);
const char* gt_function_main_symbol_name(void);
