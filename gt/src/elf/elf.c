/*
 * Copyright (c) 2026 Gaël Fortier <gael.fortier.1@ens.etsmtl.ca> *
 */

#include "elf.h"
#include "def.h"

#define GT_ELF_COPYFIELD(dst, src, field) \
    (dst).field = (src).field

static void gt_elf_hdr32to64(gt_elfhdr32* hdr32, gt_elfhdr64* hdr64)
{
    GT_ELF_COPYFIELD(*hdr64, *hdr32, e_ehsize);
    GT_ELF_COPYFIELD(*hdr64, *hdr32, e_entry);
    GT_ELF_COPYFIELD(*hdr64, *hdr32, e_flags);
    GT_ELF_COPYFIELD(*hdr64, *hdr32, e_ident);
    GT_ELF_COPYFIELD(*hdr64, *hdr32, e_machine);
    GT_ELF_COPYFIELD(*hdr64, *hdr32, e_phentsize);
    GT_ELF_COPYFIELD(*hdr64, *hdr32, e_phnum);
    GT_ELF_COPYFIELD(*hdr64, *hdr32, e_phoff);
    GT_ELF_COPYFIELD(*hdr64, *hdr32, e_shentsize);
    GT_ELF_COPYFIELD(*hdr64, *hdr32, e_shnum);
    GT_ELF_COPYFIELD(*hdr64, *hdr32, e_shoff);
    GT_ELF_COPYFIELD(*hdr64, *hdr32, e_shstrndx);
    GT_ELF_COPYFIELD(*hdr64, *hdr32, e_type);
    GT_ELF_COPYFIELD(*hdr64, *hdr32, e_version);
}

static void gt_elf_shdr32to64(gt_elfshdr32* shdr32, gt_elfshdr64* shdr64)
{
    GT_ELF_COPYFIELD(*shdr64, *shdr32, sh_addr);
    GT_ELF_COPYFIELD(*shdr64, *shdr32, sh_addralign);
    GT_ELF_COPYFIELD(*shdr64, *shdr32, sh_entsize);
    GT_ELF_COPYFIELD(*shdr64, *shdr32, sh_flags);
    GT_ELF_COPYFIELD(*shdr64, *shdr32, sh_info);
    GT_ELF_COPYFIELD(*shdr64, *shdr32, sh_link);
    GT_ELF_COPYFIELD(*shdr64, *shdr32, sh_name);
    GT_ELF_COPYFIELD(*shdr64, *shdr32, sh_offset);
    GT_ELF_COPYFIELD(*shdr64, *shdr32, sh_size);
    GT_ELF_COPYFIELD(*shdr64, *shdr32, sh_type);
}

static void gt_elf_phdr32to64(gt_elfphdr32* phdr32, gt_elfphdr64* phdr64)
{
    GT_ELF_COPYFIELD(*phdr64, *phdr32, p_align);
    GT_ELF_COPYFIELD(*phdr64, *phdr32, p_filesz);
    GT_ELF_COPYFIELD(*phdr64, *phdr32, p_flags);
    GT_ELF_COPYFIELD(*phdr64, *phdr32, p_memsz);
    GT_ELF_COPYFIELD(*phdr64, *phdr32, p_filesz);
    GT_ELF_COPYFIELD(*phdr64, *phdr32, p_offset);
    GT_ELF_COPYFIELD(*phdr64, *phdr32, p_paddr);
    GT_ELF_COPYFIELD(*phdr64, *phdr32, p_type);
    GT_ELF_COPYFIELD(*phdr64, *phdr32, p_vaddr);
}

int gt_elf_getident(gt_buf* buf, gt_elfident* out_ident)
{
    GT_PRECOND(out_ident == NULL, GT_ELF_INVALID_OUT);

    if (gt_buf_reatat(buf, 0, sizeof(*out_ident), out_ident))
        return gt_trace_foward(GT_HERE);

    return GT_ELF_OK;
}

int gt_elf_gethdr(gt_buf* buf, gt_elfident* ident, gt_elfhdr64* out_hdr)
{
    GT_PRECOND(ident == NULL, GT_ELF_INVALID_IDENT);
    GT_PRECOND(out_hdr == NULL, GT_ELF_INVALID_OUT);
    GT_PRECOND(ident->ei_class == GT_ELFCLASS_NONE, GT_ELF_INVALID_CLASS);

    size_t  is_64bits = ident->ei_class - 1;
    size_t  size = (is_64bits) ? sizeof(gt_elfhdr64) : sizeof(gt_elfhdr32);
    uint8_t bytes[size];

    if (gt_buf_reatat(buf, 0, size, bytes))
        return gt_trace_foward(GT_HERE);

    (is_64bits)
        ? memcpy(out_hdr, bytes, size)
        : gt_elf_hdr32to64((gt_elfhdr32*)bytes, out_hdr);

    return GT_ELF_OK;
}

int gt_elf_getshdr(gt_buf* buf, gt_elfhdr64* hdr, size_t index, gt_elfshdr64* out_shdr)
{
    GT_PRECOND(hdr == NULL, GT_ELF_INVALID_IDENT);
    GT_PRECOND(index >= hdr->e_shnum, GT_ELF_INVALID_INDEX);
    GT_PRECOND(out_shdr == NULL, GT_ELF_INVALID_OUT);
    GT_PRECOND(hdr->e_ident.ei_class == GT_ELFCLASS_NONE, GT_ELF_INVALID_CLASS)

    size_t  is_64bits = hdr->e_ident.ei_class - 1;
    size_t  size = (is_64bits) ? sizeof(gt_elfshdr64) : sizeof(gt_elfshdr32);
    size_t  offset = hdr->e_shoff + index * size;
    uint8_t bytes[size];

    if (gt_buf_reatat(buf, offset, size, bytes))
        return gt_trace_foward(GT_HERE);

    (is_64bits)
        ? memcpy(out_shdr, bytes, size)
        : gt_elf_shdr32to64((gt_elfshdr32*)bytes, out_shdr);

    return GT_ELF_OK;
}

int gt_elf_getphdr(gt_buf* buf, gt_elfhdr64* hdr, size_t index, gt_elfphdr64* out_phdr)
{
    GT_PRECOND(hdr == NULL, GT_ELF_INVALID_IDENT);
    GT_PRECOND(index >= hdr->e_phnum, GT_ELF_INVALID_INDEX);
    GT_PRECOND(out_phdr == NULL, GT_ELF_INVALID_OUT);
    GT_PRECOND(hdr->e_ident.ei_class == GT_ELFCLASS_NONE, GT_ELF_INVALID_CLASS)

    size_t  is_64bits = hdr->e_ident.ei_class - 1;
    size_t  size = (is_64bits) ? sizeof(gt_elfphdr64) : sizeof(gt_elfphdr32);
    size_t  offset = hdr->e_phoff + index * size;
    uint8_t bytes[size];

    if (gt_buf_reatat(buf, offset, size, bytes))
        return gt_trace_foward(GT_HERE);

    (is_64bits)
        ? memcpy(out_phdr, bytes, size)
        : gt_elf_phdr32to64((gt_elfphdr32*)bytes, out_phdr);

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
