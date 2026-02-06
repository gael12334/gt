/*
 *  Copyright (c) 2026 Gaël Fortier <gael.fortier.1@ens.etsmtl.ca>
 */

#include "shdr.h"
#include "asserts.h"
#include "ehdr.h"
#include <stdio.h>

int gt_elf32_get_shdr_array(gt_buf* buf, size_t* out_num, gt_elf32_shdr** out_shdr)
{
    GT_TRYTHIS(gt_elf_assert_out(out_shdr, sizeof(void*)));
    GT_TRYTHIS(gt_elf_assert_out(out_num, sizeof(size_t)));

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

int gt_elf32_get_shdr_segment(gt_buf* buf, gt_elf32_shdr* shdr, gt_buf* out_segment)
{
    GT_TRYTHIS(gt_elf_assert_class(buf, GT_ELF_IDENT_CLASS_32));
    GT_THROWIF(shdr == NULL, GT_ELF_INVALID_SHDR);
    GT_TRYTHIS(gt_elf_assert_out(out_segment, sizeof(gt_buf)));
    GT_TRYTHIS(gt_buf_segment(buf, shdr->offset, shdr->size, out_segment));
    return GT_ELF_OK;
}

int gt_elf32_get_shdr_by_type(gt_buf* buf, gt_elf_shdr_type type, gt_elf32_shdr* prev, gt_elf32_shdr** out_shdr)
{
    GT_TRYTHIS(gt_elf_assert_class(buf, GT_ELF_IDENT_CLASS_32));
    GT_THROWIF(type >= GT_ELF_SHDR_TYPE_NUM, GT_ELF_INVALID_SHTYPE);
    GT_TRYTHIS(gt_elf_assert_out(out_shdr, sizeof(void*)));

    gt_elf32_shdr* shdr = NULL;
    size_t         num = 0;
    GT_TRYTHIS(gt_elf32_get_shdr_array(buf, &num, &shdr));

    gt_elf32_shdr* cur = shdr;
    gt_elf32_shdr* end = &shdr[num];
    if (prev != NULL) {
        GT_TRYTHIS(gt_elf_assert_arrayref(shdr, end, prev, sizeof(gt_elf32_shdr)));
        cur = prev + 1;
    }

    while (cur != end) {
        if (cur->type == type) {
            *out_shdr = cur;
            return GT_ELF_OK;
        }
        cur++;
    }

    return GT_ELF_RESULTS_NOT_FOUND;
}

int gt_elf32_get_shdr_by_name(gt_buf* buf, gt_buf* name, gt_elf32_shdr** out_shdr)
{
    GT_TRYTHIS(gt_elf_assert_class(buf, GT_ELF_IDENT_CLASS_32));
    GT_TRYTHIS(gt_elf_assert_out(out_shdr, sizeof(void*)));
    GT_THROWIF(name == NULL, GT_ELF_INVALID_NAME);

    gt_elf32_ehdr* ehdr = NULL;
    GT_TRYTHIS(gt_elf32_get_header(buf, &ehdr));

    gt_elf32_shdr* shdr = NULL;
    size_t         num = 0;
    GT_TRYTHIS(gt_elf32_get_shdr_array(buf, &num, &shdr));

    gt_buf         strtab_seg = GT_BUF_ZEROED;
    gt_elf32_shdr* strtab_sh = &shdr[ehdr->shstrndx];
    GT_TRYTHIS(gt_elf32_get_shdr_segment(buf, strtab_sh, &strtab_seg));

    gt_elf32_shdr* cur = shdr;
    gt_elf32_shdr* end = shdr + num;
    while (cur != end) {
        size_t name_size = 0;
        GT_TRYTHIS(gt_buf_size(name, &name_size));

        gt_buf cur_seg = GT_BUF_ZEROED;
        GT_TRYTHIS(gt_buf_segment(&strtab_seg, cur->name, name_size, &cur_seg));

        int equals = 0;
        GT_TRYTHIS(gt_buf_equals(&cur_seg, name, &equals));

        if (equals) {
            *out_shdr = cur;
            return GT_ELF_OK;
        }
        cur++;
    }

    return GT_ELF_RESULTS_NOT_FOUND;
}

int gt_elf32_print_shdr(const gt_elf32_shdr* shdr)
{
    GT_THROWIF(shdr == NULL, GT_ELF_INVALID_SHDR);
    printf("name %u\n", shdr->name);
    printf("type %u\n", shdr->type);
    printf("flags %u\n", shdr->flags);
    printf("addr %u\n", shdr->addr);
    printf("offset %u\n", shdr->offset);
    printf("size %u\n", shdr->size);
    printf("link %u\n", shdr->link);
    printf("info %u\n", shdr->info);
    printf("addralign %u\n", shdr->addralign);
    printf("entsize %u\n", shdr->entsize);
    return GT_ELF_OK;
}
