/*
 * Copyright (c) 2026 Gaël Fortier <gael.fortier.1@ens.etsmtl.ca>
 */

#pragma once
#include "../buf/buf.h"
#include "../trace/trace.h"
#include "def.h"
#include <stdint.h>

typedef enum {
    GT_ELF_OK,
    GT_ELF_INVALID_OUT,
    GT_ELF_INVALID_IDENT,
    GT_ELF_INVALID_CLASS,
    GT_ELF_INVALID_INDEX,
    GT_ELF_INVALID_HDR,
    GT_ELF_INVALID_SHDR,
    GT_ELF_INVALID_SIZE,
    GT_ELF_INVALID_BUF,
    GT_ELF_INVALID_TABLE,
    GT_ELF_INVALID_ENTINFO,
    GT_ELF_MESSAGE_CONTINUE,
    GT_ELF_MESSAGE_STOP,
} gt_elf_result;

typedef struct {
    uint32_t num;
    uint32_t esz;
    union {
        uint32_t off;
        uint32_t ndx;
    };
} gt_elf_entinfo32;

typedef struct {
    uint64_t index;
    void*    value;
} gt_elf_iterator;

/***********************************
 *        Global functions
 *         Header 32 bits
 ***********************************/
int gt_elf_getident(gt_buf* buf, gt_elfident* out);
int gt_elf_getheader32(gt_buf* buf, gt_elfhdr32* out);

/***********************************
 *        Global functions
 *         Tables 32 bits
 ***********************************/
int gt_elf_getshdr32table(gt_buf* buf, gt_elfhdr32* hdr, gt_elfshdr32** out);
int gt_elf_getphdr32table(gt_buf* buf, gt_elfhdr32* hdr, gt_elfphdr32** out);
int gt_elf_getsym32table(gt_buf* buf, gt_elfshdr32* shdr, gt_elfsym32** out);

/***********************************
 *        Global functions
 *     Table element 32 bits
 ***********************************/
int gt_elf_getshdr32at(gt_elfhdr32* hdr, gt_elfshdr32* table, size_t index, gt_elfshdr32** out);
int gt_elf_getphdr32at(gt_elfhdr32* hdr, gt_elfphdr32* table, size_t index, gt_elfphdr32** out);
int gt_elf_getsym32at(gt_elfshdr32* shdr, gt_elfsym32* table, size_t index, gt_elfsym32** out);

/***********************************
 *        Global functions
 *      Section data 32 bits
 ***********************************/
int gt_elf_getshdr32rel(gt_buf* buf, gt_elfhdr32* hdr, gt_elfrel32** out);   // TODO
int gt_elf_getshdr32rela(gt_buf* buf, gt_elfhdr32* hdr, gt_elfrela32** out); // TODO
