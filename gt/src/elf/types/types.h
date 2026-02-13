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
#include <stdint.h>

/*************************
 *  ELF primitive types
 *************************/

typedef uint8_t gt_elf_byte;
typedef uint16_t gt_elf_half;
typedef uint32_t gt_elf_word;
typedef uint64_t gt_elf_xword;
typedef int32_t gt_elf_sword;
typedef int64_t gt_elf_sxword;
typedef uint32_t gt_elf32_addr;
typedef uint32_t gt_elf32_off;
typedef uint64_t gt_elf64_addr;
typedef uint64_t gt_elf64_off;

/*************************
 *   ELF section index
 *************************/

typedef enum : gt_elf_half {
  GT_ELF_SHN_UNDEF = 0,
  GT_ELF_SHN_LORESERVE = 0xff00,
  GT_ELF_SHN_LOPROC = 0xff00,
  GT_ELF_SHN_HIPROC = 0xff1f,
  GT_ELF_SHN_ABS = 0xfff1,
  GT_ELF_SHN_COMMON = 0xfff2,
  GT_ELF_SHN_HIRESERVE = 0xffff,
} gt_elf_shn;
