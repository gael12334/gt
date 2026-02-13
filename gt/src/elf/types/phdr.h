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

#pragma onces
#include "types.h"

/*************************
 *  Program header types
 *************************/

typedef enum {
  GT_ELF_PHDR_TYPE_NULL,
  GT_ELF_PHDR_TYPE_LOAD,
  GT_ELF_PHDR_TYPE_DYNAMIC,
  GT_ELF_PHDR_TYPE_INTERP,
  GT_ELF_PHDR_TYPE_NOTE,
  GT_ELF_PHDR_TYPE_SHLIB,
  GT_ELF_PHDR_TYPE_PHDR,
  GT_ELF_PHDR_TYPE_TLS,
  GT_ELF_PHDR_TYPE_NUM,
  GT_ELF_PHDR_TYPE_LOOS = 0x60000000,
  GT_ELF_PHDR_TYPE_HIOS = 0x6fffffff,
  GT_ELF_PHDR_TYPE_LOPROC = 0x70000000,
  GT_ELF_PHDR_TYPE_HIPROC = 0x7fffffff
} gt_elf_phdr_type;

typedef enum {
  GT_ELF_PHDR_FLAG_X = 0x1,
  GT_ELF_PHDR_FLAG_W = 0x2,
  GT_ELF_PHDR_FLAG_R = 0x4,
  GT_ELF_PHDR_FLAG_MASKOS = 0X0ff00000,
  GT_ELF_PHDR_FLAG_MASKPROC = 0Xf0000000
} gt_elf_phdr_flags;

typedef struct {
  gt_elf_word type;
  gt_elf32_off offset;
  gt_elf32_addr vaddr, paddr;
  gt_elf_word filesz, memsz;
  gt_elf_word flags, align;
} gt_elf32_phdr;

typedef struct {
  gt_elf_word type, flags;
  gt_elf64_off offset;
  gt_elf64_addr vaddr, paddr;
  gt_elf_xword filesz, memsz, align;
} gt_elf64_phdr;
