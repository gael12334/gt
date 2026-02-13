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
  gt_elf_word name;
  gt_elf32_addr addr;
  gt_elf_word size;
  gt_elf_byte info;
  gt_elf_byte other;
  gt_elf_shn shndx;
} gt_elf32_sym;

typedef struct {
  gt_elf_word name;
  gt_elf_byte info;
  gt_elf_byte other;
  gt_elf_shn shndx;
  gt_elf64_addr addr;
  gt_elf_xword size;
} gt_elf64_sym;
