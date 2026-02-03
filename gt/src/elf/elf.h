/*
 *  Copyright (c) 2026 Gaël Fortier <gael.fortier.1@ens.etsmtl.ca>
 */

/*
 * References
 * https://gabi.xinuos.com/elf.pdf
 * https://refspecs.linuxfoundation.org/elf/gabi4+/ch4.eheader.html
 */

#pragma once
#include "../buf/buf.h"
#include "../trace/trace.h"
#include <stdint.h>

/*************************
 *  ELF primitive types
 *************************/

typedef uint8_t  gt_elf_byte;
typedef uint16_t gt_elf_half;
typedef uint32_t gt_elf_word;
typedef uint64_t gt_elf_xword;
typedef int32_t  gt_elf_sword;
typedef int64_t  gt_elf_sxword;
typedef uint32_t gt_elf32_addr;
typedef uint32_t gt_elf32_off;
typedef uint64_t gt_elf64_addr;
typedef uint64_t gt_elf64_off;

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

typedef enum : gt_elf_half {
    GT_ELF_SHN_UNDEF = 0,
    GT_ELF_SHN_LORESERVE = 0xff00,
    GT_ELF_SHN_LOPROC = 0xff00,
    GT_ELF_SHN_HIPROC = 0xff1f,
    GT_ELF_SHN_ABS = 0xfff1,
    GT_ELF_SHN_COMMON = 0xfff2,
    GT_ELF_SHN_HIRESERVE = 0xffff,
} gt_elf_shn;

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
 *  Section header types
 *************************/

typedef enum : gt_elf_word {
    GT_ELF_SHDR_TYPE_NULL,
    GT_ELF_SHDR_TYPE_PROGBITS,
    GT_ELF_SHDR_TYPE_SYMTAB,
    GT_ELF_SHDR_TYPE_STRTAB,
    GT_ELF_SHDR_TYPE_RELA,
    GT_ELF_SHDR_TYPE_HASH,
    GT_ELF_SHDR_TYPE_DYNAMIC,
    GT_ELF_SHDR_TYPE_NOTE,
    GT_ELF_SHDR_TYPE_NOBITS,
    GT_ELF_SHDR_TYPE_REL,
    GT_ELF_SHDR_TYPE_SHLIB,
    GT_ELF_SHDR_TYPE_DYNSYM,
    GT_ELF_SHDR_TYPE_LOPROC = 0x70000000,
    GT_ELF_SHDR_TYPE_HIPROC = 0x7fffffff,
    GT_ELF_SHDR_TYPE_LOUSER = 0x80000000,
    GT_ELF_SHDR_TYPE_HIUSER = 0xffffffff
} gt_elf_shdr_type;

typedef enum : gt_elf_word {
    GT_ELF_SHDR_FLAG_WRITE = 0x1,
    GT_ELF_SHDR_FLAG_ALLOC = 0x2,
    GT_ELF_SHDR_FLAG_EXECINSTR = 0x4,
    GT_ELF_SHDR_FLAG_MASKPROC = 0xf0000000
} gt_elf_shdr_flag;

typedef struct {
    gt_elf_word   name, type, flags;
    gt_elf32_addr addr;
    gt_elf32_off  offset;
    gt_elf_word   size;
    gt_elf_word   link, info;
    gt_elf_word   addralign, entsize;
} gt_elf32_shdr;

typedef struct {
    gt_elf_word   name, type;
    gt_elf_xword  flags;
    gt_elf64_addr addr;
    gt_elf64_off  offset;
    gt_elf_xword  size;
    gt_elf_word   link, info;
    gt_elf_xword  addralign, entsize;
} gt_elf64_shdr;

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
    gt_elf_word   name;
    gt_elf32_addr addr;
    gt_elf_word   size;
    gt_elf_byte   info;
    gt_elf_byte   other;
    gt_elf_shn    shndx;
} gt_elf32_sym;

typedef struct {
    gt_elf_word   name;
    gt_elf_byte   info;
    gt_elf_byte   other;
    gt_elf_shn    shndx;
    gt_elf64_addr addr;
    gt_elf_xword  size;
} gt_elf64_sym;

/*************************
 *  Program header types
 *************************/

typedef enum {
    GT_ELF_PHDR_TYPE_ENUM_NULL,
    GT_ELF_PHDR_TYPE_ENUM_LOAD,
    GT_ELF_PHDR_TYPE_ENUM_DYNAMIC,
    GT_ELF_PHDR_TYPE_ENUM_INTERP,
    GT_ELF_PHDR_TYPE_ENUM_NOTE,
    GT_ELF_PHDR_TYPE_ENUM_SHLIB,
    GT_ELF_PHDR_TYPE_ENUM_PHDR,
    GT_ELF_PHDR_TYPE_ENUM_TLS,
    GT_ELF_PHDR_TYPE_ENUM_LOOS = 0x60000000,
    GT_ELF_PHDR_TYPE_ENUM_HIOS = 0x6fffffff,
    GT_ELF_PHDR_TYPE_ENUM_LOPROC = 0x70000000,
    GT_ELF_PHDR_TYPE_ENUM_HIPROC = 0x7fffffff
} gt_elf_phdr_type;

typedef enum {
    GT_ELF_PHDR_FLAG_ENUM_X = 0x1,
    GT_ELF_PHDR_FLAG_ENUM_W = 0x2,
    GT_ELF_PHDR_FLAG_ENUM_R = 0x4,
    GT_ELF_PHDR_FLAG_ENUM_MASKOS = 0X0ff00000,
    GT_ELF_PHDR_FLAG_ENUM_MASKPROC = 0Xf0000000
} gt_elf_phdr_flags;

typedef struct {
    gt_elf_word   type;
    gt_elf32_off  offset;
    gt_elf32_addr vaddr, paddr;
    gt_elf_word   filesz, memsz;
    gt_elf_word   flags, align;
} gt_elf32_phdr;

typedef struct {
    gt_elf_word   type, flags;
    gt_elf64_off  offset;
    gt_elf64_addr vaddr, paddr;
    gt_elf_xword  filesz, memsz, align;
} gt_elf64_phdr;

/*************************
 *   Module error code
 *************************/

typedef enum {
    GT_ELF_OK,
    GT_ELF_INVALID_ELF,
    GT_ELF_INVALID_OUT,
    GT_ELF_INVALID_NAME,
    GT_ELF_INVALID_EHDR,
    GT_ELF_INVALID_SHDR,
    GT_ELF_INVALID_SIZE,
    GT_ELF_INVALID_CLASS,
    GT_ELF_INVALID_MAGIC,
    GT_ELF_INVALID_LOOKUP,
    GT_ELF_FAILURE_MALLOC,
    GT_ELF_FAILURE_LOADED,
    GT_ELF_INVALID_SHSTRNDX,
    GT_ELF_INVALID_SHSTRTAB,
    GT_ELF_FAILURE_UNLOADED,
    GT_ELF_FAILURE_NOT_IMPLEMENTED,
} gt_elf_result;

/*************************
 * Module 32 bit functions
 *************************/

int gt_elf_get_ident(gt_buf* buf, gt_elf_ident** out_ident);
int gt_elf32_get_header(gt_buf* buf, gt_elf32_ehdr** out_ehdr);
int gt_elf32_get_shdr_array(gt_buf* buf, size_t* out_num, gt_elf32_shdr** out_shdr);
int gt_elf32_get_phdr_array(gt_buf* buf, size_t* out_num, gt_elf32_phdr** out_phdr);
int gt_elf32_get_sym_array(gt_buf* buf, gt_elf32_shdr* shdr, size_t* out_num, gt_elf32_sym** out_sym, gt_elf_shn* out_ndx);
int gt_elf32_get_shdr_segment(gt_buf* buf, gt_elf32_shdr* shdr, gt_buf* out_segment);
int gt_elf32_get_phdr_segment(gt_buf* buf, gt_elf32_phdr* shdr, gt_buf* out_segment);
int gt_elf32_get_sym_segment(gt_buf* buf, gt_elf32_sym* sym, gt_buf* out_segment);
int gt_elf32_get_str_segment(gt_buf* buf, gt_elf32_shdr* shdr, gt_buf* out_segment);
int gt_elf32_get_shdr_by_type(gt_buf* buf, gt_elf_shdr_type type, gt_elf32_shdr* prev, gt_elf32_shdr** out_shdr);
int gt_elf32_get_phdr_by_type(gt_buf* buf, gt_elf_phdr_type type, gt_elf32_sym* prev, gt_elf32_sym** out_sym);
int gt_elf32_get_sym_by_type(gt_buf* buf, gt_elf_sym_type type, gt_elf32_sym* prev, gt_elf32_sym** out_sym);
int gt_elf32_get_shdr_by_name(gt_buf* buf, gt_buf* name, gt_elf32_shdr** out_shdr);
int gt_elf32_get_sym_by_name(gt_buf* buf, gt_buf* name, gt_elf32_sym** out_sym);
