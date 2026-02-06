/*
 *  Copyright (c) 2026 Gaël Fortier <gael.fortier.1@ens.etsmtl.ca>
 */

#pragma once
#include "../../buf/buf.h"
#include "../../trace/trace.h"
#include "types.h"

/*************************
 *   ELF identity types
 *************************/

typedef enum : gt_elf_byte {
    GT_ELF_IDENT_CLASS_NONE,
    GT_ELF_IDENT_CLASS_32,
    GT_ELF_IDENT_CLASS_64
} gt_elf_ident_class;

typedef enum : gt_elf_byte {
    GT_ELF_IDENT_DATA_NONE,
    GT_ELF_IDENT_DATA_2LSB,
    GT_ELF_IDENT_DATA_2MSB
} gt_elf_ident_data;

typedef enum : gt_elf_byte {
    GT_ELF_IDENT_VERSION_NONE,
    GT_ELF_IDENT_VERSION_CURRENT
} gt_elf_ident_version;

typedef struct {
    gt_elf_byte magic[4];
    gt_elf_byte clazz, data, version;
    gt_elf_byte osabi, abiversion;
    gt_elf_byte reserved[7];
} gt_elf_ident;

/*************************
 *      Header types
 *************************/

typedef enum : gt_elf_half {
    GT_ELF_EHDR_TYPE_NONE,
    GT_ELF_EHDR_TYPE_REL,
    GT_ELF_EHDR_TYPE_EXEC,
    GT_ELF_EHDR_TYPE_DYN,
    GT_ELF_EHDR_TYPE_CORE,
    GT_ELF_EHDR_TYPE_LOPROC = 0xff00,
    GT_ELF_EHDR_TYPE_HIPROC = 0x00ff,
} gt_elf_ehdr_type;

typedef enum : gt_elf_word {
    GT_ELF_EHDR_VERSION_NONE,
    GT_ELF_EHDR_VERSION_CURRENT
} gt_elf_ehdr_version;

typedef struct {
    gt_elf_ident  ident;
    gt_elf_half   type, machine;
    gt_elf_word   version;
    gt_elf32_addr entry;
    gt_elf32_off  phoff, shoff;
    gt_elf_word   flags;
    gt_elf_half   ehsize, phentsize, phnum;
    gt_elf_half   shentsize, shnum, shstrndx;
} gt_elf32_ehdr;

typedef struct {
    gt_elf_ident  ident;
    gt_elf_half   type, machine;
    gt_elf_word   version;
    gt_elf64_addr entry;
    gt_elf64_off  phoff, shoff;
    gt_elf_word   flags;
    gt_elf_half   ehsize, phentsize, phnum;
    gt_elf_half   shentsize, shnum, shstrndx;
} gt_elf64_ehdr;

/*************************
 * Module 32 bit functions
 *************************/

int gt_elf32_print_header(const gt_elf32_ehdr* ehdr);
int gt_elf_get_ident(gt_buf* buf, gt_elf_ident** out_ident);
int gt_elf32_get_header(gt_buf* buf, gt_elf32_ehdr** out_ehdr);
