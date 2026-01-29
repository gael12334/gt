/*
 *  Copyright (c) 2026 Gaël Fortier <gael.fortier.1@ens.etsmtl.ca>
 */

#pragma once
#include <stdint.h>
#pragma push pack(1)
#define gt_elf_nothing
#define gt_elf_symvalue (0)
#define gt_elf_symsize (1)

/*
 * Reference :
 *
 * elf(5) - Linux manual page. (n.d.). Man7.org.
 * https://man7.org/linux/man-pages/man5/elf.5.html
 */

#define gt_elf_ehdr_X(X)      \
    gt_elf_ident e_ident;     \
    uint16_t     e_type;      \
    uint16_t     e_machine;   \
    uint32_t     e_version;   \
    uint##X##_t  e_entry;     \
    uint##X##_t  e_phoff;     \
    uint##X##_t  e_shoff;     \
    uint32_t     e_flags;     \
    uint16_t     e_ehsize;    \
    uint16_t     e_phentsize; \
    uint16_t     e_phnum;     \
    uint16_t     e_shentsize; \
    uint16_t     e_shnum;     \
    uint16_t     e_shstrndx;

#define gt_elf_phdr_X(X, flag_before, flag_after) \
    uint32_t p_type;                              \
    flag_before;                                  \
    uint##X##_t p_offset;                         \
    uint##X##_t p_vaddr;                          \
    uint##X##_t p_paddr;                          \
    uint##X##_t p_filesz;                         \
    uint##X##_t p_memsz;                          \
    flag_after;                                   \
    uint##X##_t p_align;

#define gt_elf_shdr_X(X)      \
    uint32_t    sh_name;      \
    uint32_t    sh_type;      \
    uint##X##_t sh_flags;     \
    uint##X##_t sh_addr;      \
    uint##X##_t sh_offset;    \
    uint##X##_t sh_size;      \
    uint32_t    sh_link;      \
    uint32_t    sh_info;      \
    uint##X##_t sh_addralign; \
    uint##X##_t sh_entsize;

#define gt_elf_symvalsz_X(X) \
    uint##X##_t st_value;    \
    uint##X##_t st_size;

#define gt_elf_sym_X(X, valsz_before, valsz_after) \
    uint32_t st_name;                              \
    valsz_before;                                  \
    uint8_t  st_info;                              \
    uint8_t  st_other;                             \
    uint16_t st_shndx;                             \
    valsz_after

#define gt_elf_rel_X(X)   \
    uint##X##_t r_offset; \
    uint##X##_t r_info;

#define gt_elf_rela_X(X)  \
    uint##X##_t r_offset; \
    uint##X##_t r_info;   \
    int##X##_t  r_addend;

#define gt_elf_dyn_X(X)    \
    uint##X##_t d_tag;     \
    union {                \
        uint##X##_t d_val; \
        uint##X##_t d_ptr; \
    }

typedef struct {
    uint8_t ei_mag0;
    uint8_t ei_mag1;
    uint8_t ei_mag2;
    uint8_t ei_mag3;
    uint8_t ei_class;
    uint8_t ei_data;
    uint8_t ei_version;
    uint8_t ei_osabi;
    uint8_t ei_abiversion;
    uint8_t ei_pad[7];
} gt_elf_ident;

typedef struct {
    gt_elf_ehdr_X(32);
} gt_elf_ehdr32;

typedef struct {
    gt_elf_ehdr_X(64);
} gt_elf_ehdr64;

typedef struct {
    gt_elf_phdr_X(32, gt_elf_nothing, uint32_t p_flags);
} gt_elf_phdr32;

typedef struct {
    gt_elf_phdr_X(64, uint32_t p_flags, gt_elf_nothing);
} gt_elf_phdr64;

typedef struct {
    gt_elf_shdr_X(32);
} gt_elf_shdr32;

typedef struct {
    gt_elf_shdr_X(64);
} gt_elf_shdr64;

typedef struct {
    gt_elf_sym_X(32, gt_elf_symvalsz_X(32), gt_elf_nothing)
} gt_elf_sym32;

typedef struct {
    gt_elf_sym_X(64, gt_elf_nothing, gt_elf_symvalsz_X(64));
} gt_elf_sym64;

typedef struct {
    gt_elf_rel_X(32);
} gt_elf_rel32;

typedef struct {
    gt_elf_rel_X(64);
} gt_elf_rel64;

typedef struct {
    gt_elf_rela_X(32);
} gt_elf_rela32;

typedef struct {
    gt_elf_rela_X(64);
} gt_elf_rela64;

typedef struct {
    gt_elf_dyn_X(32);
} gt_elf_dyn32;

typedef struct {
    gt_elf_dyn_X(64);
} gt_elf_dyn64;

typedef struct {
    uint32_t n_namesz;
    uint32_t n_descsz;
    uint32_t n_type;
} gt_elf_nhdr;

typedef struct {

} gt_elf;

#pragma pop
