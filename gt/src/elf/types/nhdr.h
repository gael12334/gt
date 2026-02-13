/*
 *  Copyright (c) 2026 Gaël Fortier <gael.fortier.1@ens.etsmtl.ca>
 */

/*
 *  References:
 *
 *  elf(5) - Linux manual page. (n.d.). Man7.org.
 *  https://man7.org/linux/man-pages/man5/elf.5.html Tool Interface Standard (TIS)
 *
 *  Executable and Linking Format (ELF) Specification Version 1.2. (1995).
 *  https://refspecs.linuxfoundation.org/elf/elf.pdf
 */

#pragma once
#include "types.h"

/*************************
 *     Node header
 *************************/

typedef struct {
  gt_elf_word namesz;
  gt_elf_word descsz;
  gt_elf_word type;
} gt_elf_ndr;
