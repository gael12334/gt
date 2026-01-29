/*
 *  Copyright (c) 2026 Gaël Fortier <gael.fortier.1@ens.etsmtl.ca>
 */

#pragma once
#include "ident.h"
#include <stdint.h>
#pragma push pack(1)
#define gt_elf_nothing

/*
 * Reference :
 *
 * elf(5) - Linux manual page. (n.d.). Man7.org.
 * https://man7.org/linux/man-pages/man5/elf.5.html
 */

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

typedef struct {
    gt_elf_phdr_X(32, gt_elf_nothing, uint32_t p_flags);
} gt_elf_phdr32;

typedef struct {
    gt_elf_phdr_X(64, uint32_t p_flags, gt_elf_nothing);
} gt_elf_phdr64;

#undef gt_elf_nothing
#pragma pop
