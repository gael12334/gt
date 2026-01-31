/*
 * Copyright (c) 2026 Gaël Fortier <gael.fortier.1@ens.etsmtl.ca>
 */

/*
 * References and sources
 *
 * ELF Object File Format — ELF Object File Format 4.3 DRAFT documentation. (2023). Xinuos.com.
 * https://gabi.xinuos.com/index.html
 */

#pragma once
#include <stdint.h>

typedef uint8_t  gt_elfbyte;
typedef uint16_t gt_elfhalf;
typedef uint32_t gt_elfword;
typedef uint64_t gt_elfxword;
typedef int8_t   gt_elfsbyte;
typedef int16_t  gt_elfshalf;
typedef int32_t  gt_elfsword;
typedef int64_t  gt_elfsxword;

typedef uint32_t gt_elfaddr32;
typedef uint32_t gt_elfoff32;
typedef uint64_t gt_elfaddr64;
typedef uint64_t gt_elfoff64;

typedef struct {
    gt_elfbyte ei_mag0, ei_mag1, ei_mag2, ei_mag3;
    gt_elfbyte ei_class, ei_data, ei_version;
    gt_elfbyte ei_osabi, ei_abiversion;
    gt_elfbyte ei_pad[7];
} gt_elfident;

typedef enum {
    GT_ELFCLASS_NONE,
    GT_ELFCLASS_32,
    GT_ELFCLASS_64
} gt_elfclass;

typedef enum {
    GT_ELFDATA_NONE,
    GT_ELFDATA_2LSB,
    GT_ELFDATA_2MSB,
    GT_ELFDATA_LEND = GT_ELFDATA_2LSB, // LITTLE ENDIAN (from right to left)
    GT_ELFDATA_BEND = GT_ELFDATA_2MSB  // BIG ENDIAN (from left to right)
} gt_elfdata;

typedef struct {
    gt_elfident  e_ident;
    gt_elfhalf   e_type, e_machine;
    gt_elfword   e_version;
    gt_elfaddr32 e_entry;
    gt_elfoff32  e_phoff, e_shoff;
    gt_elfword   e_flags;
    gt_elfhalf   e_ehsize;
    gt_elfhalf   e_phentsize, e_phnum;
    gt_elfhalf   e_shentsize, e_shnum;
    gt_elfhalf   e_shstrndx;
} gt_elfhdr32;

typedef struct {
    gt_elfident  e_ident;
    gt_elfhalf   e_type, e_machine;
    gt_elfword   e_version;
    gt_elfaddr64 e_entry;
    gt_elfoff64  e_phoff, e_shoff;
    gt_elfword   e_flags;
    gt_elfhalf   e_ehsize;
    gt_elfhalf   e_phentsize, e_phnum;
    gt_elfhalf   e_shentsize, e_shnum;
    gt_elfhalf   e_shstrndx;
} gt_elfhdr64;

typedef enum {
    GT_ELFTYPE_NONE,
    GT_ELFTYPE_REL,
    GT_ELFTYPE_EXEC,
    GT_ELFTYPE_DYN,
    GT_ELFTYPE_CORE,
    GT_ELFTYPE_NUM
} gt_elftype;

typedef enum {
    GT_ELFVERSION_NONE,
    GT_ELFVERSION_CURRENT
} gt_elfversion;

typedef struct {
    gt_elfword   sh_name, sh_type;
    gt_elfword   sh_flags;
    gt_elfaddr32 sh_addr;
    gt_elfoff32  sh_offset;
    gt_elfword   sh_size;
    gt_elfword   sh_link, sh_info;
    gt_elfword   sh_addralign;
    gt_elfword   sh_entsize;
} gt_elfshdr32;

typedef struct {
    gt_elfword   sh_name, sh_type;
    gt_elfxword  sh_flags;
    gt_elfaddr64 sh_addr;
    gt_elfoff64  sh_offset;
    gt_elfxword  sh_size;
    gt_elfword   sh_link, sh_info;
    gt_elfxword  sh_addralign;
    gt_elfxword  sh_entsize;
} gt_elfshdr64;

typedef enum {
    GT_ELFSHNDX_UNDEF = 0,
    GT_ELFSHNDX_ABS = 0xfff1,
    GT_ELFSHNDX_COMMON = 0xfff2,
    GT_ELFSHNDX_XINDEX = 0xffff
} gt_elfshndx;

typedef enum {
    GT_ELFSHDRTYPE_NULL,
    GT_ELFSHDRTYPE_PROGBITS,
    GT_ELFSHDRTYPE_SYMTAB,
    GT_ELFSHDRTYPE_STRTAB,
    GT_ELFSHDRTYPE_RELA,
    GT_ELFSHDRTYPE_HASH,
    GT_ELFSHDRTYPE_DYNAMIC,
    GT_ELFSHDRTYPE_NOTE,
    GT_ELFSHDRTYPE_NOBITS,
    GT_ELFSHDRTYPE_REL,
    GT_ELFSHDRTYPE_SHLIB,
    GT_ELFSHDRTYPE_DYNSYM,
    GT_ELFSHDRTYPE_INIT_ARRAY,
    GT_ELFSHDRTYPE_FINI_ARRAY,
    GT_ELFSHDRTYPE_PREINIT_ARRAY,
    GT_ELFSHDRTYPE_GROUP,
    GT_ELFSHDRTYPE_SYMTAB_SHNDX,
    GT_ELFSHDRTYPE_RELR,
    GT_ELFSHDRTYPE_NUM,
    GT_ELFSHDRTYPE_LOUSER = 0x80000000,
    GT_ELFSHDRTYPE_HIUSER = 0xffffffff
} gt_elfshdrtype;

typedef enum {
    GT_ELFSHDRFLAG_WRITE = 0x1,
    GT_ELFSHDRFLAG_ALLOC = 0x2,
    GT_ELFSHDRFLAG_EXECINSTR = 0x4,
    GT_ELFSHDRFLAG_MERGE = 0x10,
    GT_ELFSHDRFLAG_STRINGS = 0x20,
    GT_ELFSHDRFLAG_INFO_LINK = 0x40,
    GT_ELFSHDRFLAG_LINK_ORDER = 0x80,
    GT_ELFSHDRFLAG_OS_NONCONFORMING = 0x100,
    GT_ELFSHDRFLAG_GROUP = 0x200,
    GT_ELFSHDRFLAG_TLS = 0x400,
    GT_ELFSHDRFLAG_COMPRESSED = 0x800,
} gt_elfshdrflag;

typedef struct {
    gt_elfword ch_type;
    gt_elfword ch_size;
    gt_elfword ch_addralign;
} gt_elfchdr32;

typedef struct {
    gt_elfxword ch_type;
    gt_elfxword ch_reserved;
    gt_elfxword ch_size;
    gt_elfxword ch_addralign;
} gt_elfchdr64;

typedef enum {
    GT_ELFCHDRTYPE_ZLIB = 1,
    GT_ELFCHDRTYPE_ZSTD = 2,
} gt_elfchdrtype;

typedef struct {
    gt_elfword sg_flag;
    gt_elfword sg_shndx[];
} gt_elfgroup;

typedef enum {
    GT_ELFGROUPFLAG_COMDAT = 0x1
} gt_elfgroupflag;

typedef struct {
    gt_elfword   st_name;
    gt_elfaddr32 st_value;
    gt_elfword   st_size;
    gt_elfbyte   st_info, st_other;
    gt_elfhalf   st_shndx;
} gt_elfsym32;

typedef struct {
    gt_elfword   st_name;
    gt_elfbyte   st_info, st_other;
    gt_elfhalf   st_shndx;
    gt_elfaddr64 st_value;
    gt_elfxword  st_size;
} gt_elfsym64;

// https://gabi.xinuos.com/elf/05-symtab.html

typedef enum {
    GT_ELFSYMBIND_LOCAL,
    GT_ELFSYMBIND_GLOBAL,
    GT_ELFSYMBIND_WEAK
} gt_elfsymbind;

typedef enum {
    GT_ELFSYMTYPE_NOTYPE,
    GT_ELFSYMTYPE_OBJECT,
    GT_ELFSYMTYPE_FUNC,
    GT_ELFSYMTYPE_SECTION,
    GT_ELFSYMTYPE_FILE,
    GT_ELFSYMTYPE_COMMON,
    GT_ELFSYMTYPE_TLS
} gt_elfsymtype;

typedef enum {
    GT_ELFSYMVISIB_DEFAULT,
    GT_ELFSYMVISIB_INTERNAL,
    GT_ELFSYMVISIB_HIDDEN,
    GT_ELFSYMVISIB_PROTECTED,
    GT_ELFSYMVISIB_EXPORTED,
    GT_ELFSYMVISIB_SINGLETON,
    GT_ELFSYMVISIB_ELIMINATE
} gt_elfsymvisib;

typedef struct {
    gt_elfaddr32 r_offset;
    gt_elfword   r_info;
} gt_elfrel32;

typedef struct {
    gt_elfaddr64 r_offset;
    gt_elfxword  r_info;
} gt_elfrel64;

typedef struct {
    gt_elfaddr32 r_offset;
    gt_elfword   r_info;
    gt_elfsword  r_addend;
} gt_elfrela32;

typedef struct {
    gt_elfaddr64 r_offset;
    gt_elfxword  r_info;
    gt_elfsxword r_addend;
} gt_elfrela64;

// https://gabi.xinuos.com/elf/06-reloc.html

typedef struct {
    gt_elfword   p_type;
    gt_elfoff32  p_offset;
    gt_elfaddr32 p_vaddr;
    gt_elfaddr32 p_paddr;
    gt_elfword   p_filesz;
    gt_elfword   p_memsz;
    gt_elfword   p_flags;
    gt_elfword   p_align;
} gt_elfphdr32;

typedef struct {
    gt_elfword   p_type;
    gt_elfword   p_flags;
    gt_elfoff64  p_offset;
    gt_elfaddr64 p_vaddr;
    gt_elfaddr64 p_paddr;
    gt_elfxword  p_filesz;
    gt_elfxword  p_memsz;
    gt_elfxword  p_align;
} gt_elfphdr64;

typedef enum {
    GT_ELFPHDRTYPE_NULL,
    GT_ELFPHDRTYPE_LOAD,
    GT_ELFPHDRTYPE_DYNAMIC,
    GT_ELFPHDRTYPE_INTERP,
    GT_ELFPHDRTYPE_NOTE,
    GT_ELFPHDRTYPE_SHLIB,
    GT_ELFPHDRTYPE_PHDR,
    GT_ELFPHDRTYPE_TLS
} gt_elfphdrtype;

typedef enum {
    GT_ELFPHDRPERM_X = 0x1,
    GT_ELFPHDRPERM_W = 0x2,
    GT_ELFPHDRPERM_R = 0x4
} gt_elfphdrperm;

typedef struct {
    gt_elfword n_namesz;
    gt_elfword n_descsz;
    gt_elfword n_type;
    gt_elfbyte n_data[];
} gt_elfnhdr32;

typedef struct {
    gt_elfxword n_namesz;
    gt_elfxword n_descsz;
    gt_elfxword n_type;
    gt_elfbyte  n_data[];
} gt_elfnhdr64;

typedef struct {
    gt_elfsword d_tag;
    union {
        gt_elfword   d_val;
        gt_elfaddr32 d_ptr;
    };
} gt_elfdyn32;

typedef struct {
    gt_elfsxword d_tag;
    union {
        gt_elfxword  d_val;
        gt_elfaddr64 d_ptr;
    };
} gt_elfdyn64;

typedef enum {
    GT_ELFDYNTAG_NULL,
    GT_ELFDYNTAG_NEEDED,
    GT_ELFDYNTAG_PLTRELSZ,
    GT_ELFDYNTAG_PLTGOT,
    GT_ELFDYNTAG_HASH,
    GT_ELFDYNTAG_STRTAB,
    GT_ELFDYNTAG_SYMTAB,
    GT_ELFDYNTAG_RELA,
    GT_ELFDYNTAG_RELASZ,
    GT_ELFDYNTAG_RELAENT,
    GT_ELFDYNTAG_STRSZ,
    GT_ELFDYNTAG_SYMENT,
    GT_ELFDYNTAG_INIT,
    GT_ELFDYNTAG_FNIT,
    GT_ELFDYNTAG_SONAME,
    GT_ELFDYNTAG_RPATH,
    GT_ELFDYNTAG_SYMBOLIC,
    GT_ELFDYNTAG_REL,
    GT_ELFDYNTAG_RELSZ,
    GT_ELFDYNTAG_RELENT,
    GT_ELFDYNTAG_PLTREL,
    GT_ELFDYNTAG_DEBUG,
    GT_ELFDYNTAG_TEXTREL,
    GT_ELFDYNTAG_JMPREL,
    GT_ELFDYNTAG_BIND_NOW,
    GT_ELFDYNTAG_INIT_ARRAY,
    GT_ELFDYNTAG_FINI_ARRAY,
    GT_ELFDYNTAG_INIT_ARRAYSZ,
    GT_ELFDYNTAG_FINI_ARRAYSZ,
    GT_ELFDYNTAG_RUNPATH,
    GT_ELFDYNTAG_FLAGS,
    GT_ELFDYNTAG_reserved0,
    GT_ELFDYNTAG_ENCODING = 32,
    GT_ELFDYNTAG_PREINIT_ARRAY = 32,
    GT_ELFDYNTAG_PREINIT_ARRAYSZ,
    GT_ELFDYNTAG_SYMTAB_SHNDX,
    GT_ELFDYNTAG_RELRSZ,
    GT_ELFDYNTAG_RELR,
    GT_ELFDYNTAG_RELRENT,
    GT_ELFDYNTAG_reserved1,
    GT_ELFDYNTAG_SYMTABSZ
} gt_elfdyntag;

typedef enum {
    GT_ELFDYNFLAG_ORIGIN = 0x1,
    GT_ELFDYNFLAG_SYMBOLIC = 0x2,
    GT_ELFDYNFLAG_TEXTREL = 0x4,
    GT_ELFDYNFLAG_BIND_NOW = 0x8,
    GT_ELFDYNFLAG_STATIC_TLS = 0x10
} gt_elfdynflag;

typedef struct {
    gt_elfword h_nbucket;
    gt_elfword h_nchain;
    gt_elfbyte h_data[];
} gt_elfhash;

// https://gabi.xinuos.com/elf/08-dynamic.html

/*
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 */
