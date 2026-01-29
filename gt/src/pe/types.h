/*
 *  Copyright (c) 2026 Gaël Fortier <gael.fortier.1@ens.etsmtl.ca>
 */

#pragma once
#include <stdint.h>
#pragma push pack(1)
#define gt_pe_nothing

/*
 * Reference :
 *
 * https://learn.microsoft.com/en-us/windows/win32/debug/pe-format
 * https://fr.wikipedia.org/wiki/Portable_Executable
 */

#define gt_pe_stdhdr_X(X, baseofdata) \
    uint16_t s_magic;                 \
    uint8_t  s_majlnkver;             \
    uint8_t  s_minlnkver;             \
    uint32_t s_codesz;                \
    uint32_t s_initdatsz;             \
    uint32_t s_uninitdatsz;           \
    uint32_t s_entryaddr;             \
    uint32_t s_baseofcode;            \
    baseofdata;

#define gt_pe_winhdr_X(X)        \
    uint##X##_t   w_imgbase;     \
    uint32_t      w_sectalign;   \
    uint32_t      w_filealign;   \
    uint16_t      w_majosver;    \
    uint16_t      w_minosver;    \
    uint16_t      w_majimgver;   \
    uint16_t      w_minimgver;   \
    uint16_t      w_majsubver;   \
    uint16_t      w_minsubver;   \
    uint32_t      w_win32ver;    \
    uint32_t      w_imgsz;       \
    uint32_t      w_hdrsz;       \
    uint32_t      w_checksum;    \
    uint16_t      w_subsys;      \
    uint16_t      w_dllflags;    \
    uint##X##_t   w_stackrez;    \
    uint##X##_t   w_stackcommit; \
    uint##X##_t   w_heaprez;     \
    uint##X##_t   w_heap_commit; \
    uint32_t      w_ldflags;     \
    uint32_t      w_rvanum;      \
    gt_pe_datadir w_edatatab;    \
    gt_pe_datadir w_idatatab;    \
    gt_pe_datadir w_rsrctab;     \
    gt_pe_datadir w_pdatatab;    \
    gt_pe_datadir w_certtab;     \
    gt_pe_datadir w_reloctab;    \
    gt_pe_datadir w_debugsect;   \
    gt_pe_datadir w_archsect;    \
    gt_pe_datadir w_globptr;     \
    gt_pe_datadir w_tlstab;      \
    gt_pe_datadir w_ldconftab;   \
    gt_pe_datadir w_bitab;       \
    gt_pe_datadir w_iatab;       \
    gt_pe_datadir w_dlitab;      \
    gt_pe_datadir w_clrhdr;      \
    gt_pe_datadir w_reserved;

#define gt_pe_ehdr_X(X)       \
    gt_pe_coffhdr   e_coff;   \
    gt_pe_stdhdr##X e_stdhdr; \
    gt_pe_winhdr##X e_winhdr;

typedef struct {
    uint16_t e_magic;
    uint16_t e_endsz;
    uint16_t e_pagenum;
    uint16_t e_relocnum;
    uint16_t e_hdrsz;
    uint16_t e_minalloc;
    uint16_t e_maxalloc;
    uint16_t e_initss;
    uint16_t e_initsp;
    uint16_t e_checksum;
    uint16_t e_initip;
    uint16_t e_initcs;
    uint16_t e_relocaddr;
    uint16_t e_overlaynb;
    uint16_t e_reserved[4];
    uint16_t e_oemid;
    uint16_t e_oeminfo;
    uint16_t e_reserved2[10];
    uint32_t e_pehdrptr;
} gt_pe_mzhdr;

typedef struct {
    uint16_t c_machine;
    uint16_t c_sectnum;
    uint32_t c_timestamp;
    uint32_t c_symtabptr;
    uint32_t c_symnum;
    uint16_t c_opthdrsz;
    uint16_t c_flags;
} gt_pe_coffhdr;

typedef struct {
    uint32_t d_vaddr;
    uint32_t d_size;
} gt_pe_datadir;

typedef struct {
    gt_pe_stdhdr_X(32, uint32_t s_baseofdata)
} gt_pe_stdhdr32;

typedef struct {
    gt_pe_stdhdr_X(64, gt_pe_nothing);
} gt_pe_stdhdr64;

typedef struct {
    gt_pe_winhdr_X(32);
} gt_pe_winhdr32;

typedef struct {
    gt_pe_winhdr_X(64);
} gt_pe_winhdr64;

typedef struct {
    gt_pe_ehdr_X(32);
} gt_pe_ehdr32;

typedef struct {
    gt_pe_ehdr_X(64);
} gt_pe_ehdr64;

typedef struct {
    uint8_t  s_name[8];
    uint32_t s_vsize;
    uint32_t s_vaddr;
    uint32_t s_rawdatasz;
    uint32_t s_rawdataptr;
    uint32_t s_relocptr;
    uint32_t s_linenumptr;
    uint32_t s_relocnum;
    uint32_t s_linenumnum;
    uint32_t s_flags;
} gt_pe_shdr;

typedef struct {
    uint32_t r_vaddr;
    uint32_t r_symtabndx;
    uint16_t r_type;
} gt_pe_coffreloc;

typedef struct {
    union {
        uint32_t l_symtabndx;
        uint32_t l_vaddr;
    } type;
    uint16_t l_linenum;
} gt_pe_cofflinenum;

typedef struct {
    union {
        uint8_t s_shortname;
        struct {
            uint32_t s_zeroes;
            uint32_t s_offset;
        };
    };
    uint32_t s_value;
    uint16_t s_sectnum;
    uint16_t s_type;
    uint8_t  s_storage;
    uint8_t  s_auxsymnum;
} gt_pe_coffsym;

typedef union {
    struct gt_pe_auxfn {
        uint32_t a_tagndx;
        uint32_t a_size;
        uint32_t a_lnptr;
        uint32_t a_nextptr;
        uint16_t a_unused;
    } a_function;
    struct gt_pe_auxbe {
        uint32_t a_unused;
        uint16_t a_linenum;
        uint8_t  a_unused2[6];
        uint32_t a_nextptr;
        uint16_t a_unused3;
    } a_beginfunc, a_endfunc;
    struct gt_pe_auxwe {
        uint32_t a_tagndx;
        uint32_t a_flags;
        uint8_t  a_unused[10];
    } a_weakext;
    struct gt_pe_auxfile {
        uint8_t a_filename[18];
    } a_file;
    struct gt_pe_auxsectdef {
        uint32_t a_length;
        uint16_t a_relocnum;
        uint16_t a_linenumnum;
        uint32_t a_checksum;
        uint32_t a_number;
        uint8_t  a_selection;
        uint8_t  a_unused[3];
    } a_section;
    struct gt_pe_clrtok {
        uint8_t  a_auxtype;
        uint8_t  a_reserved;
        uint32_t a_symndx;
        uint8_t  a_reserved2[12];
    } a_clrtoken;
} gt_pe_auxsym;

typedef struct {
    uint32_t d_attrib;
    uint32_t d_nameptr;
    uint32_t d_moduleptr;
    uint32_t d_diatab;
    uint32_t d_dintab;
    uint32_t d_bditab;
    uint32_t d_timestamp;
} gt_pe_;

typedef struct {

} gt_pe_export;

#pragma pop
