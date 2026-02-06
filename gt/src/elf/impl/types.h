/*
 *  Copyright (c) 2026 Gaël Fortier <gael.fortier.1@ens.etsmtl.ca>
 */
#pragma once
#include <stdint.h>

/*************************
 *  ELF primitive types
 *************************/

typedef uint8_t  gt_elf_byte;
typedef uint16_t gt_elf_half;
typedef uint32_t gt_elf_word;
typedef uint64_t gt_elf_xword;
typedef int32_t  gt_elf_sword;
typedef int64_t  gt_elf_sxword;
typedef uint32_t gt_elf32_addr;
typedef uint32_t gt_elf32_off;
typedef uint64_t gt_elf64_addr;
typedef uint64_t gt_elf64_off;

/*************************
 *   ELF section index
 *************************/

typedef enum : gt_elf_half {
    GT_ELF_SHN_UNDEF = 0,
    GT_ELF_SHN_LORESERVE = 0xff00,
    GT_ELF_SHN_LOPROC = 0xff00,
    GT_ELF_SHN_HIPROC = 0xff1f,
    GT_ELF_SHN_ABS = 0xfff1,
    GT_ELF_SHN_COMMON = 0xfff2,
    GT_ELF_SHN_HIRESERVE = 0xffff,
} gt_elf_shn;

/*************************
 *   Module error code
 *************************/

typedef enum {
    GT_ELF_OK,
    GT_ELF_INVALID_ELF,
    GT_ELF_INVALID_OUT,
    GT_ELF_INVALID_PTR,
    GT_ELF_INVALID_SYM,
    GT_ELF_INVALID_NAME,
    GT_ELF_INVALID_EHDR,
    GT_ELF_INVALID_SHDR,
    GT_ELF_INVALID_PHDR,
    GT_ELF_INVALID_SIZE,
    GT_ELF_INVALID_CLASS,
    GT_ELF_INVALID_MAGIC,
    GT_ELF_INVALID_LOOKUP,
    GT_ELF_INVALID_SHTYPE,
    GT_ELF_INVALID_PHTYPE,
    GT_ELF_INVALID_SHSTRNDX,
    GT_ELF_INVALID_SHSTRTAB,
    GT_ELF_INVALID_SYMSTRNDX,
    GT_ELF_FAILURE_MALLOC,
    GT_ELF_FAILURE_LOADED,
    GT_ELF_FAILURE_UNLOADED,
    GT_ELF_FAILURE_NOT_IMPLEMENTED,
    GT_ELF_RESULTS_NOT_FOUND,
} gt_elf_result;
