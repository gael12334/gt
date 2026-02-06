/*
 *  Copyright (c) 2026 Gaël Fortier <gael.fortier.1@ens.etsmtl.ca>
 */

#include "asserts.h"
#include "ehdr.h"

int gt_elf_assert_class(gt_buf* buf, int clazz)
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

int gt_elf_assert_out(void* addr, size_t size)
{
    GT_THROWIF(addr == NULL, GT_ELF_INVALID_OUT);

    gt_buf buf = GT_BUF_ZEROED;
    GT_TRYTHIS(gt_buf_init(size, addr, &buf));

    int zeroed = 0;
    GT_TRYTHIS(gt_buf_zeroed(&buf, &zeroed));
    GT_THROWIF(zeroed != 1, GT_ELF_INVALID_OUT);

    return GT_ELF_OK;
}

int gt_elf_assert_arrayref(void* start, void* end, void* ptr, size_t size)
{
    uintptr_t i_start = (uintptr_t)start,
              i_end = (uintptr_t)end,
              i_ptr = (uintptr_t)ptr;

    GT_THROWIF(i_ptr < i_start, GT_ELF_INVALID_PTR);
    GT_THROWIF(i_ptr > i_end, GT_ELF_INVALID_PTR);

    size_t diff = i_ptr - i_start;
    while (diff >= size) {
        diff -= size;
    }

    GT_THROWIF(diff > 0, GT_ELF_INVALID_PTR);
    return GT_ELF_OK;
}
