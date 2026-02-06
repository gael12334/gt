/*
 *  Copyright (c) 2026 Gaël Fortier <gael.fortier.1@ens.etsmtl.ca>
 */

#include "../file/file.h"
#include "elf.h"
#include <stdio.h>

static uint8_t MAGIC[] = { 0x7f, 'E', 'L', 'F' };
static gt_buf  ELF = GT_BUF_ZEROED;

static void load_ELF(void)
{
    ELF = GT_BUF_ZEROED;
    if (gt_file_load("out/sample.bin", &ELF)) {
        printf("'out/sample.bin' is absent\n");
        exit(-1);
    }
}

static void unload_ELF(void)
{
    if (gt_file_unload(&ELF)) {
        printf("error while unloading file\n");
        exit(-1);
    }
}

static void test_gt_elf_get_ident(void)
{
    // arrange
    load_ELF();
    gt_elf_ident* ident = NULL;

    // act
    int result = gt_elf_get_ident(&ELF, &ident);

    // assert
    if (result)
        printf("result was %i\n", result);
    else if (ident->clazz != GT_ELF_IDENT_CLASS_32)
        printf("ident->clazz was %hhu\n", ident->clazz);
    else if (ident->data != GT_ELF_IDENT_DATA_2LSB)
        printf("ident->data was %hhu\n", ident->clazz);
    else if (memcmp(ident->magic, MAGIC, sizeof(ident->magic)) != 0)
        printf("ident->data was %hhu %c %c %c\n", ident->magic[0], ident->magic[1], ident->magic[2], ident->magic[3]);
    else
        printf("ok\n");

    unload_ELF();
}

static void test_gt_elf32_get_header(void)
{
    // Reference:
    // https://llvm.org/doxygen/namespacellvm_1_1ELF.html
#define ELF_EM_386 (3)

    // arrange
    load_ELF();
    gt_elf32_ehdr* ehdr = NULL;

    // act
    int result = gt_elf32_get_header(&ELF, &ehdr);

    // assert
    if (result)
        printf("result was %i\n", result);
    else if (ehdr->ehsize != sizeof(gt_elf32_ehdr))
        printf("ehdr->ehsize was %hu\n", ehdr->ehsize);
    else if (ehdr->machine != ELF_EM_386)
        printf("machine was not ELF_EM_386\n");
    else
        printf("ok\n");

#undef ELF_EM_386
    unload_ELF();
}

static void test_gt_elf32_get_shdr_array(void)
{
    // arrange
    load_ELF();
    gt_elf32_shdr* shdr = NULL;
    size_t         num = 0;

    // act
    int result = gt_elf32_get_shdr_array(&ELF, &num, &shdr);

    // assert
    if (result)
        printf("result was %i\n", result);
    else if (num != 35)
        printf("num was %zu\n", num);
    else if (shdr == NULL)
        printf("shdr was null\n");
    else
        printf("ok\n");

    unload_ELF();
}

static void test_gt_elf32_get_shdr_by_type(void)
{
    // arrange
    load_ELF();
    gt_elf32_shdr* shdr_sym = NULL;
    gt_elf32_shdr* shdr_str = NULL;

    // act
    int result1 = gt_elf32_get_shdr_by_type(&ELF, GT_ELF_SHDR_TYPE_SYMTAB, NULL, &shdr_sym);
    int result2 = gt_elf32_get_shdr_by_type(&ELF, GT_ELF_SHDR_TYPE_STRTAB, shdr_sym, &shdr_str);

    // assert
    if (result1)
        printf("result1 was %i\n", result1);
    else if (result2)
        printf("result2 was %i\n", result2);
    else if (shdr_sym == NULL)
        printf("shdr_sym null\n");
    else if (shdr_str == NULL)
        printf("shdr_str null\n");
    else if (shdr_sym->type != GT_ELF_SHDR_TYPE_SYMTAB)
        printf("shdr_sym->type %u\n", shdr_sym->type);
    else
        printf("ok\n");

    unload_ELF();
}

static void test_gt_elf32_get_phdr_array(void)
{
    // arrange
    load_ELF();
    gt_elf32_phdr* phdr = NULL;
    size_t         num = 0;

    // act
    int result = gt_elf32_get_phdr_array(&ELF, &num, &phdr);

    // assert
    if (result)
        printf("result was %i\n", result);
    else if (num != 11)
        printf("num was %zu\n", num);
    else if (phdr == NULL)
        printf("phdr was null\n");
    else
        printf("ok\n");

    unload_ELF();
}

static void test_gt_elf32_get_sym_array(void)
{
    // arrange
    load_ELF();
    gt_elf32_shdr* shdr = NULL;
    gt_elf32_sym*  sym = NULL;
    size_t         num = 0;
    gt_elf32_get_shdr_by_type(&ELF, GT_ELF_SHDR_TYPE_SYMTAB, NULL, &shdr);

    // act
    int result = gt_elf32_get_sym_array(&ELF, shdr, &num, &sym);

    // assert
    if (result)
        printf("result was %i\n", result);
    else if (sym->name != 0) // first symbol entry is a 'null' symbol
        printf("sym had a name\n");
    else if (sym->addr != 0)
        printf("addr was not null\n");
    else
        printf("ok\n");

    unload_ELF();
}

static void test_gt_elf32_get_shdr_segment(void)
{
    // arrange
    load_ELF();
    gt_elf32_shdr* shdr = NULL;
    gt_elf32_sym*  sym = NULL;
    gt_buf         seg = GT_BUF_ZEROED;
    size_t         num = 0;
    gt_elf32_get_shdr_by_type(&ELF, GT_ELF_SHDR_TYPE_SYMTAB, NULL, &shdr);

    // act
    int result = gt_elf32_get_shdr_segment(&ELF, shdr, &seg);

    // assert
    sym = seg.data;
    if (result)
        printf("result was %i\n", result);
    else if (seg.size != shdr->size)
        printf("size was %zu\n", seg.size);
    else if (sym->addr != 0)
        printf("wrong addr\n");
    else
        printf("ok\n");
}

static void test_gt_elf32_get_sym_segment(void)
{
    // arrange
    load_ELF();
    gt_elf32_shdr* shdr = NULL;
    gt_elf32_sym*  sym = NULL;
    gt_buf         seg = GT_BUF_ZEROED;
    size_t         num = 0;
    gt_elf32_get_shdr_by_type(&ELF, GT_ELF_SHDR_TYPE_SYMTAB, NULL, &shdr);
    gt_elf32_get_sym_array(&ELF, shdr, &num, &sym);

    printf("%zu\n", num);
    int i = 35;
    gt_elf32_print_sym(&sym[i]);

    // // act
    // int result = gt_elf32_get_shdr_segment(&ELF, shdr, &seg);
    //
    // // assert
    // sym = seg.data;
    // if (result)
    //     printf("result was %i\n", result);
    // else if (seg.size != shdr->size)
    //     printf("size was %zu\n", seg.size);
    // else if (sym->addr != 0)
    //     printf("wrong addr\n");
    // else
    //     printf("ok\n");
}

int main(int argc, char** argv)
{

    test_gt_elf_get_ident();
    test_gt_elf32_get_header();
    test_gt_elf32_get_shdr_array();
    test_gt_elf32_get_shdr_by_type();
    test_gt_elf32_get_shdr_segment();
    test_gt_elf32_get_phdr_array();
    test_gt_elf32_get_sym_array();
    test_gt_elf32_get_sym_segment();

    return 0;
}
