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

typedef struct {
    gt_elf_shdr_X(32);
} gt_elf_shdr32;

typedef struct {
    gt_elf_shdr_X(64);
} gt_elf_shdr64;

#pragma pop
