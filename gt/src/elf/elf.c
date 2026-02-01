/*
 * Copyright (c) 2026 Gaël Fortier <gael.fortier.1@ens.etsmtl.ca> *
 */

#include "elf.h"
#include "def.h"

/***********************************
 *       Static functions
 *           32 bits
 ***********************************/

static int gt_elf_is32bits(gt_elfident* ident, int* out)
{
    GT_THROWIF(ident == NULL, GT_ELF_INVALID_IDENT);
    GT_THROWIF(out == NULL, GT_ELF_INVALID_OUT);

    *out = ident->ei_class == GT_ELFCLASS_32;
    return GT_ELF_OK;
}

static int gt_elf_gettable32(gt_buf* buf, gt_elf_entinfo32* entinfo, void** out)
{
    gt_buf seg = { 0 };

    GT_THROWIF(out == NULL, GT_ELF_INVALID_OUT);
    GT_THROWIF(entinfo == NULL, GT_ELF_INVALID_ENTINFO);
    GT_TRYTHIS(gt_buf_segment(buf, entinfo->off, entinfo->esz * entinfo->num, &seg));
    GT_TRYTHIS(gt_buf_data(&seg, out));
    return GT_ELF_OK;
}

static int gt_elf_gettable32elem(gt_elf_entinfo32* entinfo, void* table, void** out)
{
    GT_THROWIF(table == NULL, GT_ELF_INVALID_TABLE);
    GT_THROWIF(out == NULL, GT_ELF_INVALID_OUT);
    GT_THROWIF(entinfo == NULL, GT_ELF_INVALID_ENTINFO);
    GT_THROWIF(entinfo->ndx >= entinfo->num, GT_ELF_INVALID_INDEX);
    *out = table + entinfo->esz * entinfo->ndx;
    return GT_ELF_OK;
}

static int gt_elf_getshdr32entinfo(gt_elfhdr32* hdr, size_t ndx_off, gt_elf_entinfo32* out)
{
    GT_THROWIF(hdr == NULL, GT_ELF_INVALID_HDR);
    GT_THROWIF(out == NULL, GT_ELF_INVALID_OUT);
    out->esz = hdr->e_shentsize;
    out->num = hdr->e_shnum;
    out->ndx = ndx_off;
    return GT_ELF_OK;
}

static int gt_elf_getphdr32entinfo(gt_elfhdr32* hdr, size_t ndx_off, gt_elf_entinfo32* out)
{
    GT_THROWIF(hdr == NULL, GT_ELF_INVALID_HDR);
    GT_THROWIF(out == NULL, GT_ELF_INVALID_OUT);
    out->esz = hdr->e_phentsize;
    out->num = hdr->e_phnum;
    out->ndx = ndx_off;
    return GT_ELF_OK;
}

static int gt_elf_getsym32entinfo(gt_elfshdr32* shdr, size_t ndx_off, gt_elf_entinfo32* out)
{
    GT_THROWIF(shdr == NULL, GT_ELF_INVALID_SHDR);
    GT_THROWIF(out == NULL, GT_ELF_INVALID_OUT);
    out->esz = shdr->sh_entsize;
    out->num = shdr->sh_size / shdr->sh_entsize;
    out->ndx = ndx_off;
    return GT_ELF_OK;
}

/***********************************
 *        Global functions
 *         Header 32 bits
 ***********************************/

int gt_elf_getident(gt_buf* buf, gt_elfident* out)
{
    size_t ident_size = sizeof(gt_elfident);
    void** out_ptr = (void**)&out;
    size_t buf_size = 0;

    GT_TRYTHIS(gt_buf_size(buf, &buf_size));
    GT_THROWIF(buf_size < ident_size, GT_ELF_INVALID_BUF);
    GT_TRYTHIS(gt_buf_data(buf, out_ptr));
    return GT_ELF_OK;
}

int gt_elf_getheader32(gt_buf* buf, gt_elfhdr32* out)
{
    void** out_ptr = (void**)&out;
    size_t out_size = sizeof(gt_elfhdr32);
    size_t buf_size = 0;
    int    is32bits = 0;

    GT_THROWIF(out == NULL, GT_ELF_INVALID_OUT);

    GT_TRYTHIS(gt_elf_getident(buf, &out->e_ident));
    GT_TRYTHIS(gt_elf_is32bits(&out->e_ident, &is32bits))
    GT_THROWIF(is32bits == 0, GT_ELF_INVALID_CLASS);

    GT_TRYTHIS(gt_buf_size(buf, &buf_size));
    GT_THROWIF(buf_size < out_size, GT_ELF_INVALID_BUF);
    GT_TRYTHIS(gt_buf_data(buf, out_ptr));

    return GT_ELF_OK;
}

/***********************************
 *        Global functions
 *       ELF tables 32 bits
 ***********************************/

int gt_elf_getshdr32table(gt_buf* buf, gt_elfhdr32* hdr, gt_elfshdr32** out)
{
    gt_elf_entinfo32 entinfo = { 0 };
    GT_TRYTHIS(gt_elf_getshdr32entinfo(hdr, hdr->e_shoff, &entinfo));
    GT_TRYTHIS(gt_elf_gettable32(buf, &entinfo, (void**)out));
    return GT_ELF_OK;
}

int gt_elf_getphdr32table(gt_buf* buf, gt_elfhdr32* hdr, gt_elfphdr32** out)
{
    gt_elf_entinfo32 entinfo = { 0 };
    GT_TRYTHIS(gt_elf_getphdr32entinfo(hdr, hdr->e_phoff, &entinfo));
    GT_TRYTHIS(gt_elf_gettable32(buf, &entinfo, (void**)out));
    return GT_ELF_OK;
}

int gt_elf_getsym32table(gt_buf* buf, gt_elfshdr32* shdr, gt_elfsym32** out)
{
    gt_elf_entinfo32 entinfo = { 0 };
    GT_TRYTHIS(gt_elf_getsym32entinfo(shdr, shdr->sh_offset, &entinfo));
    GT_TRYTHIS(gt_elf_gettable32(buf, &entinfo, (void**)out));
    return GT_ELF_OK;
}

/***********************************
 *        Global functions
 *     ELF table element 32 bits
 ***********************************/

int gt_elf_getshdr32at(gt_elfhdr32* hdr, gt_elfshdr32* table, size_t index, gt_elfshdr32** out)
{
    gt_elf_entinfo32 entinfo = { 0 };
    GT_TRYTHIS(gt_elf_getshdr32entinfo(hdr, index, &entinfo));
    GT_TRYTHIS(gt_elf_gettable32elem(&entinfo, table, (void**)out));
    return GT_ELF_OK;
}

int gt_elf_getphdr32at(gt_elfhdr32* hdr, gt_elfphdr32* table, size_t index, gt_elfphdr32** out)
{
    gt_elf_entinfo32 entinfo = { 0 };
    GT_TRYTHIS(gt_elf_getphdr32entinfo(hdr, index, &entinfo));
    GT_TRYTHIS(gt_elf_gettable32elem(&entinfo, table, (void**)out));
    return GT_ELF_OK;
}

int gt_elf_getsym32at(gt_elfshdr32* shdr, gt_elfsym32* table, size_t index, gt_elfsym32** out)
{
    gt_elf_entinfo32 entinfo = { 0 };
    GT_TRYTHIS(gt_elf_getsym32entinfo(shdr, index, &entinfo));
    GT_TRYTHIS(gt_elf_gettable32elem(&entinfo, table, (void**)out));
    return GT_ELF_OK;
}

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
 */
