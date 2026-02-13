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
#include "./types/ehdr.h"
#include "./types/phdr.h"
#include "./types/shdr.h"
#include "./types/sym.h"

typedef enum {
  GT_ELF_SECTFLAG_NONE,
  GT_ELF_SECTFLAG_BSS,
  GT_ELF_SECTFLAG_COMMENT,
  GT_ELF_SECTFLAG_DATA,
  GT_ELF_SECTFLAG_DATA1,
  GT_ELF_SECTFLAG_DEBUG,
  GT_ELF_SECTFLAG_DYNAMIC,
  GT_ELF_SECTFLAG_DYNSTR,
  GT_ELF_SECTFLAG_DYNSYM,
  GT_ELF_SECTFLAG_FINI,
  GT_ELF_SECTFLAG_GOT,
  GT_ELF_SECTFLAG_INIT,
  GT_ELF_SECTFLAG_INTERP,
  GT_ELF_SECTFLAG_HASH,
  GT_ELF_SECTFLAG_LINE,
  GT_ELF_SECTFLAG_NOTE,
  GT_ELF_SECTFLAG_PLT,
  GT_ELF_SECTFLAG_RODATA,
  GT_ELF_SECTFLAG_RODATA1,
  GT_ELF_SECTFLAG_SHSTRTAB,
  GT_ELF_SECTFLAG_STRTAB,
  GT_ELF_SECTFLAG_SYMTAB,
  GT_ELF_SECTFLAG_TEXT,
} gt_elf_sectflag;

/*************************
 *   Module error code
 *************************/

typedef enum {
  GT_ELF_OK,
  GT_ELF_INVALID_ELF,
  GT_ELF_INVALID_OUT,
  GT_ELF_INVALID_PTR,
  GT_ELF_INVALID_SYM,
  GT_ELF_INVALID_NAME,
  GT_ELF_INVALID_EHDR,
  GT_ELF_INVALID_SHDR,
  GT_ELF_INVALID_PHDR,
  GT_ELF_INVALID_PATH,
  GT_ELF_INVALID_SIZE,
  GT_ELF_INVALID_DATA,
  GT_ELF_INVALID_CLASS,
  GT_ELF_INVALID_MAGIC,
  GT_ELF_INVALID_LOOKUP,
  GT_ELF_INVALID_SHTYPE,
  GT_ELF_INVALID_PHTYPE,
  GT_ELF_INVALID_SHSTRNDX,
  GT_ELF_INVALID_SHSTRTAB,
  GT_ELF_INVALID_SYMSTRNDX,
  GT_ELF_FAILURE_MALLOC,
  GT_ELF_FAILURE_LOADED,
  GT_ELF_FAILURE_UNLOADED,
  GT_ELF_FAILURE_NOT_IMPLEMENTED,
  GT_ELF_RESULTS_NOT_FOUND,
} gt_elf_result;

/*************************
 *   Module structs
 *************************/

typedef struct {
  gt_buf program;
  gt_buf header;
  gt_buf secthdrs;
  gt_buf proghdrs;

  struct {
    uint64_t flags;
    gt_buf bss;
    gt_buf comment;
    gt_buf data;
    gt_buf data1;
    gt_buf debug;
    gt_buf dynamic;
    gt_buf dynstr;
    gt_buf dynsym;
    gt_buf fini;
    gt_buf got; // global offset table
    gt_buf init;
    gt_buf interp;
    gt_buf hash;
    gt_buf line;
    gt_buf note;
    gt_buf plt; // procedure linkage table
    gt_buf rodata;
    gt_buf rodata1;
    gt_buf shstrtab;
    gt_buf strtab;
    gt_buf symtab;
    gt_buf text;
  } sections;
} gt_elf;

/*************************
 *   Module 32 bit funcs
 *************************/

int gt_elf_load(gt_buf* data, gt_elf* out);
int gt_elf_free(gt_elf* elf);
int gt_elf_save(gt_elf* elf);
int gt_elf_class(gt_elf* elf, gt_elf_ident_class* out);
int gt_elf_header(gt_elf* elf, gt_elf32_ehdr* out32, gt_elf64_ehdr* out64);
int gt_elf_section_headers32(gt_elf* elf, size_t index, gt_elf32_shdr* out);
int gt_elf_section_headers64(gt_elf* elf, size_t index, gt_elf64_shdr* out);
int gt_elf_program_headers32(gt_elf* elf, size_t index, gt_elf32_phdr* out);
int gt_elf_program_headers64(gt_elf* elf, size_t index, gt_elf64_phdr* out);
int gt_elf_special_section32(gt_elf* elf, gt_elf_sectflag flag, gt_elf32_shdr* out);
int gt_elf_special_section64(gt_elf* elf, gt_elf_sectflag flag, gt_elf64_shdr* out);
