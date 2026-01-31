/*
 * Copyright (c) 2026 Gaël Fortier <gael.fortier.1@ens.etsmtl.ca>
 */

#pragma once
#include "../buf/buf.h"
#include "../trace/trace.h"
#include "def.h"

typedef enum {
    GT_ELF_OK,
    GT_ELF_INVALID_OUT,
    GT_ELF_INVALID_IDENT,
    GT_ELF_INVALID_CLASS,
    GT_ELF_INVALID_INDEX,
} gt_elf_result;

int gt_elf_getident(gt_buf* buf, gt_elfident* out_ident);
int gt_elf_gethdr(gt_buf* buf, gt_elfident* ident, gt_elfhdr64* out_hdr);
int gt_elf_getshdr(gt_buf* buf, gt_elfhdr64* hdr, size_t index, gt_elfshdr64* out_shdr);
int gt_elf_getphdr(gt_buf* buf, gt_elfhdr64* hdr, size_t index, gt_elfphdr64* out_phdr);
