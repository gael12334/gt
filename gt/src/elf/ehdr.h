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

typedef struct {
    gt_elf_ehdr_X(32);
} gt_elf_ehdr32;

typedef struct {
    gt_elf_ehdr_X(64);
} gt_elf_ehdr64;

#pragma pop
