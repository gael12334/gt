/*
    Copyright (c) 2026 Gaël Fortier <gael.fortier.1@ens.etsmtl.ca>
*/

#pragma once
#include <stdint.h>
#include <stdio.h>
#include <time.h>

typedef enum {
    ELF_OK,
    ELF_QUIT,
    ELF_DONE,
    ELF_ERR_LOADED,
    ELF_ERR_PATH,
    ELF_ERR_MALLOC,
    ELF_ERR_UNLOADED,
    ELF_ERR_SEGFAULT,
    ELF_ERR_INDEX,
    ELF_ERR_ARGC,
    ELF_ERR_ELF,
    ELF_ERR_NOT_NUMBER,
    ELF_ERR_OFFSET,
    ELF_ERR_NOT_FOUND,
    ELF_ERR_DIV_ZERO,
    ELF_ERR_OPTION,
    ELF_ERR_TYPE,
    ELF_ERR_INVALID_SIZE,
    ELF_ERR_NULL,
    ELF_ERR_NOT_IMPL,
} gt_errorcode;

typedef struct {
    const char* file;
    const char* func;
    int         line;
    int         code;
} elf_error;

#define ELF_TRACE_SIZE 1024
typedef struct {
    elf_error trace[ELF_TRACE_SIZE];
    time_t    timestamp;
    size_t    length;
    uint64_t  old;
} elf_trace;

void elf_print_to_file_error_trace(FILE* output);

int elf_stack_error_struct(elf_error error);

void elf_reset_error_trace(void);

int elf_has_error(void);

#define elf_stack_error(code) \
    elf_stack_error_struct((elf_error) { __FILE__, __func__, __LINE__, code })

#define elf_print_error_trace()                \
    {                                          \
        elf_print_to_file_error_trace(stdout); \
    }

#define elf_check(expr)                  \
    {                                    \
        int err = (expr);                \
        if (err)                         \
            return elf_stack_error(err); \
    }
