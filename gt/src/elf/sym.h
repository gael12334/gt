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

typedef struct {
    gt_elf_sym_X(32, gt_elf_symvalsz_X(32), gt_elf_nothing)
} gt_elf_sym32;

typedef struct {
    gt_elf_sym_X(64, gt_elf_nothing, gt_elf_symvalsz_X(64));
} gt_elf_sym64;

#undef gt_elf_nothing
#pragma pop
