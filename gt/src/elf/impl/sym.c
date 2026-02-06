/*
 *  Copyright (c) 2026 Gaël Fortier <gael.fortier.1@ens.etsmtl.ca>
 */

#include "sym.h"
#include "asserts.h"
#include "ehdr.h"
#include "shdr.h"
#include <stdio.h>

int gt_elf32_get_sym_array(gt_buf* buf, gt_elf32_shdr* shdr, size_t* out_num, gt_elf32_sym** out_sym)
{
    // GT_THROWIF(shdr->addralign != _Alignof(gt_elf32_sym), GT_ELF_INVALID_SHDR);
    // *************************************************************
    // gt_elf32_sym has an addr alignment of 4 bytes;
    // should I expect that shdr->addralign must equal
    // to 4 ?
    //
    // https://refspecs.linuxbase.org/elf/gabi4+/ch4.sheader.html
    // *************************************************************

    /*
     * Some sections have address alignment constraints.  If a
     * section holds a doubleword, the system must ensure
     * doubleword alignment for the entire section.  That is, the
     * value of sh_addr must be congruent to zero, modulo the
     * value of sh_addralign.  Only zero and positive integral
     * powers of two are allowed.  The value 0 or 1 means that the
     * section has no alignment constraints.
     *
     * Reference: https://man7.org/linux/man-pages/man5/elf.5.html
     */

    GT_TRYTHIS(gt_elf_assert_class(buf, GT_ELF_IDENT_CLASS_32));
    GT_TRYTHIS(gt_elf_assert_out(out_num, sizeof(size_t)));
    GT_TRYTHIS(gt_elf_assert_out(out_sym, sizeof(void*)));
    GT_THROWIF(shdr == NULL, GT_ELF_INVALID_SHDR);
    GT_THROWIF(shdr->type != GT_ELF_SHDR_TYPE_SYMTAB, GT_ELF_INVALID_SHDR);

    // Get ELF symbol array
    void* sym = NULL;
    GT_TRYTHIS(gt_buf_address(buf, shdr->offset, shdr->size, &sym));

    // set OUT parameters
    *out_sym = sym;
    *out_num = shdr->size / shdr->entsize;
    return GT_ELF_OK;
}

int gt_elf32_get_sym_segment(gt_buf* buf, gt_elf32_sym* sym, gt_buf* out_segment)
{
    GT_TRYTHIS(gt_elf_assert_class(buf, GT_ELF_IDENT_CLASS_32));
    GT_THROWIF(sym == NULL, GT_ELF_INVALID_SHDR);
    GT_TRYTHIS(gt_elf_assert_out(out_segment, sizeof(gt_buf)));
    GT_TRYTHIS(gt_buf_segment(buf, sym->addr, sym->size, out_segment));
    return GT_ELF_OK;
}

int gt_elf32_get_sym_by_type(gt_buf* buf, gt_elf32_shdr* shsymtab, gt_elf_sym_type type, gt_elf32_sym* prev, gt_elf32_sym** out_sym)
{
    GT_TRYTHIS(gt_elf_assert_class(buf, GT_ELF_IDENT_CLASS_32));
    GT_THROWIF(type >= GT_ELF_SHDR_TYPE_NUM, GT_ELF_INVALID_SHTYPE);
    GT_TRYTHIS(gt_elf_assert_out(out_sym, sizeof(void*)));

    gt_elf32_sym* sym = NULL;
    size_t        num = 0;
    GT_TRYTHIS(gt_elf32_get_sym_array(buf, shsymtab, &num, &sym));

    gt_elf32_sym* cur = sym;
    gt_elf32_sym* end = &sym[num];
    if (prev != NULL) {
        GT_TRYTHIS(gt_elf_assert_arrayref(sym, end, prev, sizeof(gt_elf32_sym)));
        cur = prev + 1;
    }

    while (cur != end) {
        uint8_t symtype = cur->info & GT_ELF_SYM_TYPE_MASK;
        if (symtype == type) {
            *out_sym = cur;
            return GT_ELF_OK;
        }
        cur++;
    }

    return GT_ELF_RESULTS_NOT_FOUND;
}

int gt_elf32_get_sym_by_name(gt_buf* buf, gt_elf32_shdr* sym_shdr, gt_buf* name, gt_elf32_sym** out_sym)
{
    GT_TRYTHIS(gt_elf_assert_class(buf, GT_ELF_IDENT_CLASS_32));
    GT_TRYTHIS(gt_elf_assert_out(out_sym, sizeof(void*)));
    GT_THROWIF(sym_shdr == NULL, GT_ELF_INVALID_SHDR);
    GT_THROWIF(sym_shdr->type != GT_ELF_SHDR_TYPE_SYMTAB, GT_ELF_INVALID_SHTYPE);
    GT_THROWIF(name == NULL, GT_ELF_INVALID_NAME);

    gt_elf32_sym* sym = NULL;
    size_t        sym_num = 0;
    GT_TRYTHIS(gt_elf32_get_sym_array(buf, sym_shdr, &sym_num, &sym));

    gt_elf32_shdr* shdr = NULL;
    size_t         sh_num = 0;
    GT_TRYTHIS(gt_elf32_get_shdr_array(buf, &sh_num, &shdr));
    GT_THROWIF(sym_shdr->link >= sh_num, GT_ELF_INVALID_SYMSTRNDX);

    // [...] The index of the associated string table section can be found in the sh_link member.
    // Reference: https://man7.org/linux/man-pages/man5/elf.5.html
    gt_buf         strtab_seg = GT_BUF_ZEROED;
    gt_elf32_shdr* strtab_sh = &shdr[sym_shdr->link];
    GT_TRYTHIS(gt_elf32_get_shdr_segment(buf, strtab_sh, &strtab_seg));

    gt_elf32_sym* cur = sym;
    gt_elf32_sym* end = sym + sym_num;
    while (cur != end) {
        size_t name_size = 0;
        GT_TRYTHIS(gt_buf_size(name, &name_size));

        gt_buf cur_seg = GT_BUF_ZEROED;
        GT_TRYTHIS(gt_buf_segment(&strtab_seg, cur->name, name_size, &cur_seg));

        int equals = 0;
        GT_TRYTHIS(gt_buf_equals(&cur_seg, name, &equals));

        if (equals) {
            *out_sym = cur;
            return GT_ELF_OK;
        }
        cur++;
    }

    return GT_ELF_RESULTS_NOT_FOUND;
}

int gt_elf32_print_sym(const gt_elf32_sym* sym)
{
    GT_THROWIF(sym == NULL, GT_ELF_INVALID_SYM);
    printf("name %u\n", sym->name);
    printf("addr %u\n", sym->addr);
    printf("size %u\n", sym->size);
    printf("info %hhu\n", sym->info);
    printf("other %hhu\n", sym->other);
    printf("shndx %hu\n", sym->shndx);
    return GT_ELF_OK;
}
