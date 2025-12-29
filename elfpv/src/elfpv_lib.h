/*
    Copyright (c) 2025 Gaël Fortier <gael.fortier.1@ens.etsmtl.ca>
*/

#pragma once

#include <elf.h>
#include <linux/limits.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
    ELFPV_OK,
    ELFPV_QUIT,
    ELFPV_ERR_LOADED,
    ELFPV_ERR_PATH,
    ELFPV_ERR_MALLOC,
    ELFPV_ERR_UNLOADED,
    ELFPV_ERR_SEGFAULT,
    ELFPV_ERR_INDEX,
    ELFPV_ERR_ARGC,
    ELFPV_ERR_ELF,
    ELFPV_ERR_NOT_NUMBER,
    ELFPV_ERR_OFFSET,
    ELFPV_ERR_NOT_FOUND,
    ELFPV_ERR_DIV_ZERO,
    ELFPV_ERR_OPTION,
    ELFPV_ERR_TYPE,
    ELFPV_ERR_NULL,
} elfpv_error_code;

typedef struct {
    const char* file;
    const char* func;
    int line;
    int code;
} elfpv_error;

#define ELFPV_TRACE_SIZE 1024
typedef struct {
    elfpv_error trace[ELFPV_TRACE_SIZE];
    size_t length;
    uint64_t old;
} elfpv_trace;

int elfpv_stack_error_struct(elfpv_error error);

void elfpv_reset_error_trace(void);

void elfpv_print_error_trace(void);

int elfpv_elf_loaded(void);

int elfpv_load_elf(const char* path);

int elfpv_unload_elf(void);

int elfpv_save_elf(const char* path);

int elfpv_print_elf(void);

int elfpv_offset(size_t offset, size_t ref_size, void** ref_ref);

int elfpv_get_hdr(Elf64_Ehdr** hdr_ref);

int elfpv_print_header(void);

int elfpv_get_phdr_windex(size_t index, Elf64_Phdr** phdr_ref);

int elfpv_print_phdr(Elf64_Phdr* phdr);

int elfpv_get_shdr_strtab_shdr(Elf64_Shdr** shdr_ref);

int elfpv_get_shdr_windex(size_t index, Elf64_Shdr** shdr_ref);

int elfpv_get_shdr_wname(const char* name, Elf64_Shdr** shdr_ref);

int elfpv_get_shdr_wtype(Elf64_Word type, Elf64_Shdr** shdr_ref);

int elfpv_print_shdr(Elf64_Shdr* shdr);

int elfpv_strtab_shdr_type(Elf64_Shdr* strtab_sh);

int elfpv_get_strtab_shdr_text(Elf64_Shdr* strtab_sh, size_t index, const char** text_ref);

int elfpv_print_strtab_shdr_text(Elf64_Shdr* strtab_sh);

int elfpv_get_sym_strtab_shdr(Elf64_Shdr* sym_sh, Elf64_Shdr** shdr_ref);

int elfpv_get_sym_windex(Elf64_Shdr* sym_sh, size_t index, Elf64_Sym** sym_ref);

int elfpv_get_sym_wname(Elf64_Shdr* sym_sh, const char* name, Elf64_Sym** sym_ref);

int elfpv_get_sym_wtype(Elf64_Shdr* sym_sh, uint16_t type, int64_t prev_index, int64_t* index);

int elfpv_print_sym(Elf64_Shdr* sym_sh, Elf64_Sym* sym);

int elfpv_print_sym_shdr(Elf64_Shdr* sym_sh);

#define elfpv_stack_error(code) \
    elfpv_stack_error_struct((elfpv_error) { __FILE__, __func__, __LINE__, code })

#define elfpv_check(expr)                  \
    {                                      \
        int err = expr;                    \
        if (err)                           \
            return elfpv_stack_error(err); \
    }

#define elfpv_newl(x, ...) printf(x "\n", __VA_ARGS__)

#define elfpv_gnewl(format, x, z, d) \
    printf("%-30s %-12zu " format ";\n", x, z, d)

#define elfpv_unewl(x) _Generic((x),                             \
    uint8_t: elfpv_gnewl("%-12hhu", #x, sizeof(x), (uint8_t)x),  \
    uint16_t: elfpv_gnewl("%-12hu", #x, sizeof(x), (uint16_t)x), \
    uint32_t: elfpv_gnewl("%-12u", #x, sizeof(x), (uint32_t)x),  \
    uint64_t: elfpv_gnewl("%-12lu", #x, sizeof(x), (uint64_t)x))

#define elfpv_inewl(x) _Generic((x),                            \
    uint8_t: elfpv_gnewl("%-12hhi", #x, sizeof(x), (int8_t)x),  \
    uint16_t: elfpv_gnewl("%-12hi", #x, sizeof(x), (int16_t)x), \
    uint32_t: elfpv_gnewl("%-12i", #x, sizeof(x), (int32_t)x),  \
    uint64_t: elfpv_gnewl("%-12li", #x, sizeof(x), (int64_t)x))

#define elfpv_pnewl(x) \
    elfpv_gnewl("%-12p", #x, sizeof(x), x)

#define elfpv_snewl(x, n) \
    elfpv_gnewl("%-12s", #x, (size_t)n, x)

void elfpv_impl_bnewl(const char* expr, uint8_t* buff, size_t size);
#define elfpv_bnewl(x, n) elfpv_impl_bnewl(#x, x, n)
