/*
 * Copyright (c) 2026 Gaël Fortier <gael.fortier.1@ens.etsmtl.ca>
 */

#pragma once
#include "../buf/buf.h"
#include "enum.h"
#include <stdint.h>

/*
 * Reference :
 *
 * elf(5) - Linux manual page. (n.d.). Man7.org.
 * https://man7.org/linux/man-pages/man5/elf.5.html
 *
 * Matz, M., Hubička, J., Jaeger, A., & Mitchell, M. (2012).
 * System V Application Binary Interface AMD64 Architecture Processor Supplement Draft Version.
 * https://refspecs.linuxbase.org/elf/x86_64-abi-0.99.pdf
 *
 */

typedef struct {
    uint8_t  e_ident[GT_ELF_IDENT_N];
    uint16_t e_type;
    uint16_t e_machine;
    uint32_t e_version;
    uint32_t e_entry;
    uint32_t e_phoff;
    uint32_t e_shoff;
    uint32_t e_flags;
    uint16_t e_ehsize;
    uint16_t e_phentsz;
    uint16_t e_phnum;
    uint16_t e_shentsz;
    uint16_t e_shnum;
    uint16_t e_shstrndx;
} gt_elf32_ehdr;

typedef struct {
    uint8_t  e_ident[GT_ELF_IDENT_N];
    uint16_t e_type;
    uint16_t e_machine;
    uint32_t e_version;
    uint64_t e_entry;
    uint64_t e_phoff;
    uint64_t e_shoff;
    uint32_t e_flags;
    uint16_t e_ehsize;
    uint16_t e_phentsize;
    uint16_t e_phnum;
    uint16_t e_shentsize;
    uint16_t e_shnum;
    uint16_t e_shstrndx;
} gt_elf64_ehdr;

typedef struct {
    uint32_t sh_name;
    uint32_t sh_type;
    uint32_t sh_flags;
    uint32_t sh_addr;
    uint32_t sh_offset;
    uint32_t sh_size;
    uint32_t sh_link;
    uint32_t sh_info;
    uint32_t sh_addralign;
    uint32_t sh_entsize;
} gt_elf32_shdr;

typedef struct {
    uint32_t sh_name;
    uint32_t sh_type;
    uint64_t sh_flags;
    uint64_t sh_addr;
    uint64_t sh_offset;
    uint64_t sh_size;
    uint32_t sh_link;
    uint32_t sh_info;
    uint64_t sh_addralign;
    uint64_t sh_entsize;
} gt_elf64_shdr;

typedef struct {
    uint32_t st_name;
    uint32_t st_value;
    uint32_t st_size;
    uint8_t  st_info;
    uint8_t  st_other;
    uint16_t st_shndx;
} gt_elf32_sym;

typedef struct {
    uint32_t st_name;
    uint8_t  st_info;
    uint8_t  st_other;
    uint16_t st_shndx;
    uint64_t st_value;
    uint64_t st_size;
} gt_elf64_sym;

typedef struct {
    uint32_t r_offset;
    uint32_t r_info;
} gt_elf32_rel;

typedef struct {
    uint32_t r_offset;
    uint32_t r_info;
    int32_t  r_addend;
} gt_elf32_rela;

typedef struct {
    uint64_t r_offset;
    uint64_t r_info;
} gt_elf64_rel;

typedef struct {
    uint64_t r_offset;
    uint64_t r_info;
    int64_t  r_addend;
} gt_elf64_rela;

typedef struct {
    uint32_t p_type;
    uint32_t p_offset;
    uint32_t p_vaddr;
    uint32_t p_paddr;
    uint32_t p_filesz;
    uint32_t p_memsz;
    uint32_t p_flags;
    uint32_t p_align;
} gt_elf32_phdr;

typedef struct {
    uint32_t p_type;
    uint32_t p_flags;
    uint64_t p_offset;
    uint64_t p_vaddr;
    uint64_t p_paddr;
    uint64_t p_filesz;
    uint64_t p_memsz;
    uint64_t p_align;
} gt_elf64_phdr;

typedef struct {
    uint32_t n_namesz;
    uint32_t n_descsz;
    uint32_t n_type;
} gt_elf_note;

typedef struct {
    int32_t d_tag;
    union {
        uint32_t d_val;
        uint32_t d_ptr;
    } d_un;
} gt_elf32_dyn;

typedef struct {
    int64_t d_tag;
    union {
        uint64_t d_val;
        uint64_t d_ptr;
    } d_un;
} gt_elf64_dyn;

typedef struct {
    uint32_t sht_nbucket;
    uint32_t sht_nchain;
} gt_elf_symhtab;
