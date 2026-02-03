/*
 *  Copyright (c) 2026 Gaël Fortier <gael.fortier.1@ens.etsmtl.ca>
 */

#include "elf.h"

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
    gt_elf32_ehdr* header = NULL;
    GT_TRYTHIS(gt_elf32_get_header(buf, &header));
    GT_THROWIF(header->shentsize != sizeof(gt_elf32_shdr), GT_ELF_INVALID_EHDR);

    size_t size = header->shentsize * header->shnum;
    GT_TRYTHIS(gt_buf_address(buf, header->shoff, size, (void**)out_shdr))
    *out_num = header->shnum;

    return GT_ELF_OK;
}

int gt_elf32_get_phdr_array(gt_buf* buf, size_t* out_num, gt_elf32_phdr** out_phdr)
{
    gt_elf32_ehdr* header = NULL;
    GT_TRYTHIS(gt_elf32_get_header(buf, &header));
    GT_THROWIF(header->phentsize != sizeof(gt_elf32_phdr), GT_ELF_INVALID_EHDR);

    size_t size = header->phentsize * header->phnum;
    GT_TRYTHIS(gt_buf_address(buf, header->phoff, size, (void**)out_phdr))
    *out_num = header->phnum;

    return GT_ELF_OK;
}

int gt_elf32_get_sym_array(gt_buf* buf, gt_elf32_shdr* shdr, size_t* out_num, gt_elf32_sym** out_sym, gt_elf_shn* out_ndx)
{
    GT_TRYTHIS(gt_elf_assert_class(buf, GT_ELF_IDENT_CLASS_32));
    GT_THROWIF(shdr == NULL, GT_ELF_INVALID_SHDR);
    GT_THROWIF(shdr->type != GT_ELF_SHDR_TYPE_SYMTAB, GT_ELF_INVALID_SHDR);
    GT_THROWIF(out_num == NULL, GT_ELF_INVALID_OUT);
    GT_THROWIF(out_ndx == NULL, GT_ELF_INVALID_OUT);
    GT_THROWIF(out_sym == NULL, GT_ELF_INVALID_OUT);
    GT_THROWIF(*out_num != 0, GT_ELF_INVALID_OUT);
    GT_THROWIF(*out_ndx != 0, GT_ELF_INVALID_OUT);
    GT_THROWIF(*out_sym != NULL, GT_ELF_INVALID_OUT);
    // gt_elf32_sym has an alignment of 4 bytes;
    // should I expect that shdr->addralign must
    // equal to 4 ?
    //
    // https://refspecs.linuxbase.org/elf/gabi4+/ch4.sheader.html
    GT_THROWIF(shdr->addralign != _Alignof(gt_elf32_sym), GT_ELF_INVALID_SHDR);

    void* sym = NULL;
    GT_TRYTHIS(gt_buf_address(buf, shdr->offset, shdr->size, &sym));

    *out_sym = sym;
    *out_num = shdr->size / shdr->entsize;
    *out_ndx = shdr->link;

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
