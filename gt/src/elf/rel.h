/*
 *  Copyright (c) 2026 Gaël Fortier <gael.fortier.1@ens.etsmtl.ca>
 */

#pragma once
#include "ident.h"
#include <stdint.h>
#pragma push pack(1)

/*
 * Reference :
 *
 * elf(5) - Linux manual page. (n.d.). Man7.org.
 * https://man7.org/linux/man-pages/man5/elf.5.html
 */

#define gt_elf_rel_X(X)   \
    uint##X##_t r_offset; \
    uint##X##_t r_info;

#define gt_elf_rela_X(X)  \
    uint##X##_t r_offset; \
    uint##X##_t r_info;   \
    int##X##_t  r_addend;

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

#pragma pop
