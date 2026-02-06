/*
 *  Copyright (c) 2026 Gaël Fortier <gael.fortier.1@ens.etsmtl.ca>
 */

#pragma once
#include "../../buf/buf.h"
#include "../../trace/trace.h"
#include "shdr.h"
#include "types.h"

/*************************
 *     Symbol types
 *************************/

typedef enum {
    GT_ELF_SYM_BINDING_LOCAL = 0 << 4,
    GT_ELF_SYM_BINDING_GLOBAL = 1 << 4,
    GT_ELF_SYM_BINDING_WEAK = 2 << 4,
    GT_ELF_SYM_BINDING_LOPROC = 13 << 4,
    GT_ELF_SYM_BINDING_HIPROC = 15 << 4,
    GT_ELF_SYM_BINDING_MASK = 0xf0,
} gt_elf_sym_binding;

typedef enum {
    GT_ELF_SYM_TYPE_NOTYPE = 0,
    GT_ELF_SYM_TYPE_OBJECT = 1,
    GT_ELF_SYM_TYPE_FUNC = 2,
    GT_ELF_SYM_TYPE_SECTION = 3,
    GT_ELF_SYM_TYPE_FILE = 4,
    GT_ELF_SYM_TYPE_LOPROC = 13,
    GT_ELF_SYM_TYPE_HIPROC = 15,
    GT_ELF_SYM_TYPE_MASK = 0x0f
} gt_elf_sym_type;

typedef enum {
    GT_ELF_SYM_VISIB_DEFAULT = 0,
    GT_ELF_SYM_VISIB_INTERNAL = 1,
    GT_ELF_SYM_VISIB_HIDDEN = 2,
    GT_ELF_SYM_VISIB_PROTECTED = 3,
    GT_ELF_SYM_VISIB_MASK = 0x03
} gt_elf_sym_visib;

typedef struct {
    gt_elf_word   name;
    gt_elf32_addr addr;
    gt_elf_word   size;
    gt_elf_byte   info;
    gt_elf_byte   other;
    gt_elf_shn    shndx;
} gt_elf32_sym;

typedef struct {
    gt_elf_word   name;
    gt_elf_byte   info;
    gt_elf_byte   other;
    gt_elf_shn    shndx;
    gt_elf64_addr addr;
    gt_elf_xword  size;
} gt_elf64_sym;

/*************************
 * Module 32 bit functions
 *************************/

int gt_elf32_get_sym_array(gt_buf* buf, gt_elf32_shdr* shdr, size_t* out_num, gt_elf32_sym** out_sym);
int gt_elf32_get_sym_segment(gt_buf* buf, gt_elf32_sym* sym, gt_buf* out_segment);
int gt_elf32_get_sym_by_type(gt_buf* buf, gt_elf32_shdr* shsymtab, gt_elf_sym_type type, gt_elf32_sym* prev, gt_elf32_sym** out_sym);
int gt_elf32_get_sym_by_name(gt_buf* buf, gt_elf32_shdr* sym_shdr, gt_buf* name, gt_elf32_sym** out_sym);
int gt_elf32_print_sym(const gt_elf32_sym* sym);
