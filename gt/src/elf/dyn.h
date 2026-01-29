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

#define gt_elf_dyn_X(X)    \
    uint##X##_t d_tag;     \
    union {                \
        uint##X##_t d_val; \
        uint##X##_t d_ptr; \
    }

typedef struct {
    gt_elf_dyn_X(32);
} gt_elf_dyn32;

typedef struct {
    gt_elf_dyn_X(64);
} gt_elf_dyn64;

#pragma pop
