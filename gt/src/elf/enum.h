/*
 * Copyright (c) 2026 Gaël Fortier <gael.fortier.1@ens.etsmtl.ca>
 */

#pragma once
#include <stddef.h>

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

typedef enum {
    GT_ELF_IDENT_MAG0 = 0,
    GT_ELF_IDENT_MAG1 = 1,
    GT_ELF_IDENT_MAG2 = 2,
    GT_ELF_IDENT_MAG3 = 3,
    GT_ELF_IDENT_CLASS = 4,
    GT_ELF_IDENT_DATA = 5,
    GT_ELF_IDENT_VERSION = 6,
    GT_ELF_IDENT_OSABI = 7,
    GT_ELF_IDENT_ABIVERSION = 8,
    GT_ELF_IDENT_PAD = 9,
    GT_ELF_IDENT_N = 16,
} gt_elf_ident;

typedef enum {
    GT_ELF_PHDR_TYPE_NULL = 0,
    GT_ELF_PHDR_TYPE_LOAD = 1,
    GT_ELF_PHDR_TYPE_DYNAMIC = 2,
    GT_ELF_PHDR_TYPE_INTERP = 3,
    GT_ELF_PHDR_TYPE_NOTE = 4,
    GT_ELF_PHDR_TYPE_SHLIB = 5,
    GT_ELF_PHDR_TYPE_PHDR = 6,
    GT_ELF_PHDR_TYPE_TLS = 7,
    GT_ELF_PHDR_TYPE_NUM = 8
} gt_elf_phdr_type;
