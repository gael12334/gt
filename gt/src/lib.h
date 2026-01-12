/*
    Copyright (c) 2026 Gaël Fortier <gael.fortier.1@ens.etsmtl.ca>
*/

#pragma once

#include <elf.h>
#include <linux/limits.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef const uint8_t elf_constbyte;
typedef elf_constbyte* elf_ro_bytebuff;
typedef uint8_t elf_byte;
typedef elf_byte* elf_bytebuffer;

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
} elf_error_code;

typedef struct {
    const char* file;
    const char* func;
    int line;
    int code;
} elf_error;

#define ELF_TRACE_SIZE 1024
typedef struct {
    elf_error trace[ELF_TRACE_SIZE];
    size_t length;
    uint64_t old;
    time_t timestamp;
} elf_trace;

typedef struct {
    void* object;
    uint64_t index;
    uint64_t done;
} elf_index_iterator;

int elf_reset_index_iterator(elf_index_iterator* iterator_ref);

int elf_stack_error_struct(elf_error error);

void elf_reset_error_trace(void);

int elf_has_error(void);

#define elf_print_error_trace() elf_print_to_file_error_trace(stdout)
void elf_print_to_file_error_trace(FILE* output);

int elf_elf_loaded(void);

int elf_load_elf(const char* path);

int elf_unload_elf(void);

int elf_save_elf(const char* path);

int elf_print_elf(void);

int elf_offset(size_t offset, size_t ref_size, void** ref_ref);

int elf_get_offset(const void* end, const void* start, size_t* offset_ref);

int elf_get_hdr(Elf64_Ehdr** hdr_ref);

int elf_print_header(void);

int elf_get_phdr_windex(size_t index, Elf64_Phdr** phdr_ref);

int elf_print_phdr(Elf64_Phdr* phdr);

int elf_get_shdr_strtab_shdr(Elf64_Shdr** shdr_ref);

int elf_get_shdr_windex(size_t index, Elf64_Shdr** shdr_ref);

int elf_get_shdr_wname(const char* name, Elf64_Shdr** shdr_ref);

int elf_get_shdr_wtype(Elf64_Word type, Elf64_Shdr** shdr_ref);

int elf_print_shdr(Elf64_Shdr* shdr);

int elf_strtab_shdr_type(Elf64_Shdr* strtab_sh);

int elf_get_strtab_shdr_text(Elf64_Shdr* strtab_sh, size_t index, const char** text_ref);

int elf_print_strtab_shdr_text(Elf64_Shdr* strtab_sh);

int elf_sym_shdr_type(Elf64_Shdr* sym_sh);

int elf_sym_type(Elf64_Sym* sym, uint16_t type);

int elf_get_sym_num(Elf64_Shdr* sym_sh, size_t* num_ref);

int elf_get_sym_strtab_shdr(Elf64_Shdr* sym_sh, Elf64_Shdr** shdr_ref);

int elf_get_sym_shdr(Elf64_Shdr** shdr_ref);

int elf_get_sym_windex(Elf64_Shdr* sym_sh, size_t index, Elf64_Sym** sym_ref);

int elf_get_sym_wname(Elf64_Shdr* sym_sh, const char* name, Elf64_Sym** sym_ref);

int elf_get_sym_wtype(Elf64_Shdr* sym_sh, uint16_t type, elf_index_iterator* iterator_ref);

int elf_get_sym_offset(Elf64_Sym* sym, size_t* offset_ref);

int elf_get_readonly_sym_bytes(Elf64_Sym* sym, elf_ro_bytebuff* const buf_ref, size_t* size);

int elf_set_sym_bytes(Elf64_Sym* sym, size_t offset, elf_bytebuffer buf, size_t size);

int elf_print_sym(Elf64_Shdr* sym_sh, Elf64_Sym* sym);

int elf_print_sym_bytes(Elf64_Sym* sym, elf_ro_bytebuff bytes, size_t size);

int elf_print_sym_bytes_2(Elf64_Sym* sym);

int elf_print_sym_shdr(Elf64_Shdr* sym_sh);

#define elf_stack_error(code) \
    elf_stack_error_struct((elf_error) { __FILE__, __func__, __LINE__, code })

#define elf_check(expr)                  \
    {                                    \
        int err = expr;                  \
        if (err)                         \
            return elf_stack_error(err); \
    }

#define elf_newl(x, ...) printf(x "\n", __VA_ARGS__)

#define elf_gnewl(format, x, z, d) \
    printf("%-30s %-12zu " format "\n", x, z, d)

#define elf_unewl(x) _Generic((x),                             \
    uint8_t: elf_gnewl("%-12hhu", #x, sizeof(x), (uint8_t)x),  \
    uint16_t: elf_gnewl("%-12hu", #x, sizeof(x), (uint16_t)x), \
    uint32_t: elf_gnewl("%-12u", #x, sizeof(x), (uint32_t)x),  \
    uint64_t: elf_gnewl("%-12lu", #x, sizeof(x), (uint64_t)x))

#define elf_inewl(x) _Generic((x),                            \
    uint8_t: elf_gnewl("%-12hhi", #x, sizeof(x), (int8_t)x),  \
    uint16_t: elf_gnewl("%-12hi", #x, sizeof(x), (int16_t)x), \
    uint32_t: elf_gnewl("%-12i", #x, sizeof(x), (int32_t)x),  \
    uint64_t: elf_gnewl("%-12li", #x, sizeof(x), (int64_t)x))

#define elf_pnewl(x) \
    elf_gnewl("%-12p", #x, sizeof(x), x)

#define elf_snewl(x, n) \
    elf_gnewl("%-12s", #x, (size_t)n, x)

void elf_impl_bnewl(const char* expr, uint8_t* buff, size_t size);
#define elf_bnewl(x, n) elf_impl_bnewl(#x, x, n)
