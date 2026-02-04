/*
 *  Copyright (c) 2026 Gaël Fortier <gael.fortier.1@ens.etsmtl.ca>
 */

#include "elf.h"

#include <stdio.h>

static int gt_elf_assert_class(gt_buf* buf, gt_elf_ident_class clazz)
{
    gt_elf_ident* ident = NULL;
    GT_TRYTHIS(gt_elf_get_ident(buf, &ident));
    GT_THROWIF(ident->magic[0] != 0x7f, GT_ELF_INVALID_MAGIC);
    GT_THROWIF(ident->magic[1] != 'E', GT_ELF_INVALID_MAGIC);
    GT_THROWIF(ident->magic[2] != 'L', GT_ELF_INVALID_MAGIC);
    GT_THROWIF(ident->magic[3] != 'F', GT_ELF_INVALID_MAGIC);
    GT_THROWIF(ident->clazz != clazz, GT_ELF_INVALID_CLASS);

    return GT_ELF_OK;
}

static int gt_elf_assert_out(void* addr, size_t size)
{
    GT_THROWIF(addr == NULL, GT_ELF_INVALID_OUT);

    gt_buf buf = { 0 };
    GT_TRYTHIS(gt_buf_init(size, addr, &buf));

    int zeroed = 0;
    GT_TRYTHIS(gt_buf_zeroed(&buf, &zeroed));
    GT_THROWIF(zeroed != 1, GT_ELF_INVALID_OUT);

    return GT_ELF_OK;
}

int gt_elf_get_ident(gt_buf* buf, gt_elf_ident** out_ident)
{
    GT_TRYTHIS(gt_buf_address(buf, 0, sizeof(gt_elf_ident), (void**)out_ident));
    return GT_ELF_OK;
}

int gt_elf32_get_header(gt_buf* buf, gt_elf32_ehdr** out_ehdr)
{
    GT_TRYTHIS(gt_elf_assert_class(buf, GT_ELF_IDENT_CLASS_32));
    GT_TRYTHIS(gt_buf_address(buf, 0, sizeof(gt_elf32_ehdr), (void**)out_ehdr));
    return GT_ELF_OK;
}

int gt_elf32_get_shdr_array(gt_buf* buf, size_t* out_num, gt_elf32_shdr** out_shdr)
{
    GT_TRYTHIS(gt_elf_assert_out(out_shdr, sizeof(*out_shdr)));
    GT_TRYTHIS(gt_elf_assert_out(out_num, sizeof(*out_num)));

    // get ELF header
    gt_elf32_ehdr* header = NULL;
    GT_TRYTHIS(gt_elf32_get_header(buf, &header));
    GT_THROWIF(header->shentsize != sizeof(gt_elf32_shdr), GT_ELF_INVALID_EHDR);

    // Get ELF section header array
    void*  shdr = NULL;
    size_t size = header->shentsize * header->shnum;
    GT_TRYTHIS(gt_buf_address(buf, header->shoff, size, &shdr));

    // set OUT parameters
    *out_shdr = shdr;
    *out_num = header->shnum;
    return GT_ELF_OK;
}

int gt_elf32_get_phdr_array(gt_buf* buf, size_t* out_num, gt_elf32_phdr** out_phdr)
{
    GT_TRYTHIS(gt_elf_assert_out(out_phdr, sizeof(*out_phdr)));
    GT_TRYTHIS(gt_elf_assert_out(out_num, sizeof(*out_num)));

    // Get ELF header
    gt_elf32_ehdr* header = NULL;
    GT_TRYTHIS(gt_elf32_get_header(buf, &header));
    GT_THROWIF(header->phentsize != sizeof(gt_elf32_phdr), GT_ELF_INVALID_EHDR);

    // Get ELF program header array
    void*  phdr = NULL;
    size_t size = header->phentsize * header->phnum;
    GT_TRYTHIS(gt_buf_address(buf, header->phoff, size, &phdr));

    // set OUT parameters
    *out_phdr = phdr;
    *out_num = header->phnum;
    return GT_ELF_OK;
}

int gt_elf32_get_sym_array(gt_buf* buf, gt_elf32_shdr* shdr, size_t* out_num, gt_elf32_sym** out_sym, gt_elf_shn* out_ndx)
{
    // GT_THROWIF(shdr->addralign != _Alignof(gt_elf32_sym), GT_ELF_INVALID_SHDR);
    // *************************************************************
    // gt_elf32_sym has an addr alignment of 4 bytes;
    // should I expect that shdr->addralign must equal
    // to 4 ?
    //
    // https://refspecs.linuxbase.org/elf/gabi4+/ch4.sheader.html
    // *************************************************************

    GT_TRYTHIS(gt_elf_assert_class(buf, GT_ELF_IDENT_CLASS_32));
    GT_TRYTHIS(gt_elf_assert_out(out_num, sizeof(*out_num)));
    GT_TRYTHIS(gt_elf_assert_out(out_ndx, sizeof(*out_ndx)));
    GT_TRYTHIS(gt_elf_assert_out(out_sym, sizeof(*out_sym)));
    GT_THROWIF(shdr == NULL, GT_ELF_INVALID_SHDR);
    GT_THROWIF(shdr->type != GT_ELF_SHDR_TYPE_SYMTAB, GT_ELF_INVALID_SHDR);

    // Get ELF symbol array
    void* sym = NULL;
    GT_TRYTHIS(gt_buf_address(buf, shdr->offset, shdr->size, &sym));

    // set OUT parameters
    *out_sym = sym;
    *out_ndx = shdr->link;
    *out_num = shdr->size / shdr->entsize;
    return GT_ELF_OK;
}

int gt_elf32_get_shdr_segment(gt_buf* buf, gt_elf32_shdr* shdr, gt_buf* out_segment)
{

    return GT_ELF_FAILURE_NOT_IMPLEMENTED;
}

int gt_elf32_get_phdr_segment(gt_buf* buf, gt_elf32_phdr* shdr, gt_buf* out_segment)
{
    return GT_ELF_FAILURE_NOT_IMPLEMENTED;
}

int gt_elf32_get_sym_segment(gt_buf* buf, gt_elf32_sym* sym, gt_buf* out_segment)
{
    return GT_ELF_FAILURE_NOT_IMPLEMENTED;
}

int gt_elf32_get_str_segment(gt_buf* buf, gt_elf32_shdr* shdr, gt_buf* out_segment)
{
    return GT_ELF_FAILURE_NOT_IMPLEMENTED;
}

int gt_elf32_get_shdr_by_type(gt_buf* buf, gt_elf_shdr_type type, gt_elf32_shdr* prev, gt_elf32_shdr** out_shdr)
{
    return GT_ELF_FAILURE_NOT_IMPLEMENTED;
}

int gt_elf32_get_phdr_by_type(gt_buf* buf, gt_elf_phdr_type type, gt_elf32_sym* prev, gt_elf32_sym** out_sym)
{
    return GT_ELF_FAILURE_NOT_IMPLEMENTED;
}

int gt_elf32_get_sym_by_type(gt_buf* buf, gt_elf_sym_type type, gt_elf32_sym* prev, gt_elf32_sym** out_sym)
{
    return GT_ELF_FAILURE_NOT_IMPLEMENTED;
}

int gt_elf32_get_shdr_by_name(gt_buf* buf, gt_buf* name, gt_elf32_shdr** out_shdr)
{
    return GT_ELF_FAILURE_NOT_IMPLEMENTED;
}

int gt_elf32_get_sym_by_name(gt_buf* buf, gt_buf* name, gt_elf32_sym** out_sym)
{
    return GT_ELF_FAILURE_NOT_IMPLEMENTED;
}
