/*
 *  Copyright (c) 2026 Gaël Fortier <gael.fortier.1@ens.etsmtl.ca>
 */

#pragma once
#include "../../buf/buf.h"
#include "../../trace/trace.h"
#include "types.h"

/*************************
 *  Section header types
 *************************/

typedef enum : gt_elf_word {
    GT_ELF_SHDR_TYPE_NULL,
    GT_ELF_SHDR_TYPE_PROGBITS,
    GT_ELF_SHDR_TYPE_SYMTAB,
    GT_ELF_SHDR_TYPE_STRTAB,
    GT_ELF_SHDR_TYPE_RELA,
    GT_ELF_SHDR_TYPE_HASH,
    GT_ELF_SHDR_TYPE_DYNAMIC,
    GT_ELF_SHDR_TYPE_NOTE,
    GT_ELF_SHDR_TYPE_NOBITS,
    GT_ELF_SHDR_TYPE_REL,
    GT_ELF_SHDR_TYPE_SHLIB,
    GT_ELF_SHDR_TYPE_DYNSYM,
    GT_ELF_SHDR_TYPE_NUM,
    GT_ELF_SHDR_TYPE_LOPROC = 0x70000000,
    GT_ELF_SHDR_TYPE_HIPROC = 0x7fffffff,
    GT_ELF_SHDR_TYPE_LOUSER = 0x80000000,
    GT_ELF_SHDR_TYPE_HIUSER = 0xffffffff
} gt_elf_shdr_type;

typedef enum : gt_elf_word {
    GT_ELF_SHDR_FLAG_WRITE = 0x1,
    GT_ELF_SHDR_FLAG_ALLOC = 0x2,
    GT_ELF_SHDR_FLAG_EXECINSTR = 0x4,
    GT_ELF_SHDR_FLAG_MASKPROC = 0xf0000000
} gt_elf_shdr_flag;

typedef struct {
    gt_elf_word   name, type, flags;
    gt_elf32_addr addr;
    gt_elf32_off  offset;
    gt_elf_word   size;
    gt_elf_word   link, info;
    gt_elf_word   addralign, entsize;
} gt_elf32_shdr;

typedef struct {
    gt_elf_word   name, type;
    gt_elf_xword  flags;
    gt_elf64_addr addr;
    gt_elf64_off  offset;
    gt_elf_xword  size;
    gt_elf_word   link, info;
    gt_elf_xword  addralign, entsize;
} gt_elf64_shdr;

/*************************
 * Module 32 bit functions
 *************************/

int gt_elf32_get_shdr_array(gt_buf* buf, size_t* out_num, gt_elf32_shdr** out_shdr);
int gt_elf32_get_shdr_segment(gt_buf* buf, gt_elf32_shdr* shdr, gt_buf* out_segment);
int gt_elf32_get_shdr_by_type(gt_buf* buf, gt_elf_shdr_type type, gt_elf32_shdr* prev, gt_elf32_shdr** out_shdr);
int gt_elf32_get_shdr_by_name(gt_buf* buf, gt_buf* name, gt_elf32_shdr** out_shdr);
int gt_elf32_print_shdr(const gt_elf32_shdr* shdr);
