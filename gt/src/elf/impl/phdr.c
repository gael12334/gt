/*
 *  Copyright (c) 2026 Gaël Fortier <gael.fortier.1@ens.etsmtl.ca>
 */

#include "phdr.h"
#include "asserts.h"
#include "ehdr.h"
#include <stdio.h>

int gt_elf32_get_phdr_array(gt_buf* buf, size_t* out_num, gt_elf32_phdr** out_phdr)
{

    GT_TRYTHIS(gt_elf_assert_out(out_phdr, sizeof(void*)));
    GT_TRYTHIS(gt_elf_assert_out(out_num, sizeof(size_t)));

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

int gt_elf32_get_phdr_segment(gt_buf* buf, gt_elf32_phdr* phdr, gt_buf* out_segment)
{
    /*
     * This member holds the value to which the segments are
     * aligned in memory and in the file.  Loadable process
     * segments must have congruent values for p_vaddr and
     * p_offset, modulo the page size.  Values of zero and one
     * mean no alignment is required.  Otherwise, p_align should
     * be a positive, integral power of two, and p_vaddr should
     * equal p_offset, modulo p_align.
     *
     * Reference: https://man7.org/linux/man-pages/man5/elf.5.html
     *
     */

    // GT_TRYTHIS(gt_elf_assert_class(buf, GT_ELF_IDENT_CLASS_32));
    // GT_THROWIF(phdr == NULL, GT_ELF_INVALID_PHDR);
    // GT_TRYTHIS(gt_buf_segment(buf, phdr->, , out_segment));
    return GT_ELF_FAILURE_NOT_IMPLEMENTED;
}

int gt_elf32_get_phdr_by_type(gt_buf* buf, gt_elf_phdr_type type, gt_elf32_phdr* prev, gt_elf32_phdr** out_phdr)
{
    GT_TRYTHIS(gt_elf_assert_class(buf, GT_ELF_IDENT_CLASS_32));
    GT_THROWIF(type >= GT_ELF_PHDR_TYPE_NUM, GT_ELF_INVALID_PHTYPE);
    GT_TRYTHIS(gt_elf_assert_out(out_phdr, sizeof(void*)));

    gt_elf32_phdr* phdr = NULL;
    size_t         num = 0;
    GT_TRYTHIS(gt_elf32_get_phdr_array(buf, &num, &phdr));

    gt_elf32_phdr* cur = phdr;
    gt_elf32_phdr* end = &phdr[num];
    if (prev != NULL) {
        GT_TRYTHIS(gt_elf_assert_arrayref(phdr, end, prev, sizeof(gt_elf32_phdr)));
        cur = prev + 1;
    }

    while (cur != end) {
        if (cur->type == type) {
            *out_phdr = cur;
            return GT_ELF_OK;
        }
        cur++;
    }

    return GT_ELF_RESULTS_NOT_FOUND;
}

int gt_elf32_print_phdr(const gt_elf32_phdr* phdr)
{
    GT_THROWIF(phdr == NULL, GT_ELF_INVALID_PHDR);
    printf("type %u\n", phdr->type);
    printf("offset %u\n", phdr->offset);
    printf("vaddr %u\n", phdr->vaddr);
    printf("paddr %u\n", phdr->paddr);
    printf("filesz %u\n", phdr->filesz);
    printf("memsz %u\n", phdr->memsz);
    printf("flags %u\n", phdr->flags);
    printf("align %u\n", phdr->align);
    return GT_ELF_OK;
}
