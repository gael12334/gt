/*
 *  Copyright (c) 2026 Gaël Fortier <gael.fortier.1@ens.etsmtl.ca>
 */

#include "ehdr.h"
#include "asserts.h"
#include <stdio.h>

int gt_elf32_print_header(const gt_elf32_ehdr* ehdr)
{
    GT_THROWIF(ehdr == NULL, GT_ELF_INVALID_EHDR);
    printf("type %hu\n", ehdr->type);
    printf("machine %hu\n", ehdr->machine);
    printf("version %u\n", ehdr->version);
    printf("entry %u\n", ehdr->entry);
    printf("phoff %u\n", ehdr->phoff);
    printf("shoff %u\n", ehdr->shoff);
    printf("flags %x\n", ehdr->flags);
    printf("ehsize %hu\n", ehdr->ehsize);
    printf("phentsize %hu\n", ehdr->phentsize);
    printf("phnum %hu\n", ehdr->phnum);
    printf("shentsize %hu\n", ehdr->shentsize);
    printf("shnum %hu\n", ehdr->shnum);
    printf("shstrndx %hu\n", ehdr->shstrndx);
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
