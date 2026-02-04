/*
 *  Copyright (c) 2026 Gaël Fortier <gael.fortier.1@ens.etsmtl.ca>
 */

#include "elf.h"
#include <stdio.h>

static uint8_t ELF_RO_MAGIC[4] = {
    0x7f,
    'E',
    'L',
    'F'
};

#define DEFAULT_NUM (5)
static struct {
    gt_elf32_ehdr ehdr;
    gt_elf32_shdr shdr[DEFAULT_NUM];
    gt_elf32_phdr phdr[DEFAULT_NUM];
    gt_elf32_sym  sym[DEFAULT_NUM];
} ELF_RO_SAMPLE = { 0 };

static void init_elf_ro_sample(void)
{
    gt_elf32_ehdr* ehdr = &ELF_RO_SAMPLE.ehdr;
    ehdr->ehsize = sizeof(*ehdr);
    ehdr->machine = 1;
    ehdr->entry = 1;
    ehdr->ident.clazz = GT_ELF_IDENT_CLASS_32;
    ehdr->shentsize = sizeof(gt_elf32_shdr);
    ehdr->shoff = (void*)&ELF_RO_SAMPLE.shdr[0] - (void*)&ELF_RO_SAMPLE;
    ehdr->shnum = DEFAULT_NUM;
    ehdr->phentsize = sizeof(gt_elf32_phdr);
    ehdr->phoff = (void*)&ELF_RO_SAMPLE.phdr[0] - (void*)&ELF_RO_SAMPLE;
    ehdr->phnum = DEFAULT_NUM;
    ehdr->ident.magic[0] = 0x7f;
    ehdr->ident.magic[1] = 'E';
    ehdr->ident.magic[2] = 'L';
    ehdr->ident.magic[3] = 'F';

    gt_elf32_shdr* shdr = &ELF_RO_SAMPLE.shdr[0];
    shdr->addr = (void*)&ELF_RO_SAMPLE.sym[0] - (void*)&ELF_RO_SAMPLE;
    shdr->entsize = sizeof(gt_elf32_sym);

    // TODO : to complete.
}

void test_gt_elf_get_ident(void)
{
    // arrange
    gt_elf_ident* result = NULL;
    gt_elf_ident  ident = { 0 };
    gt_buf        buf = { .data = &ident, .size = sizeof(ident) };

    ident.clazz = GT_ELF_IDENT_CLASS_32;
    ident.data = GT_ELF_IDENT_DATA_2LSB;

    // act
    int code = gt_elf_get_ident(&buf, &result);

    // assert
    if (code) {
        printf("code was %i\n", code);
    } else if (memcmp(&ident, result, sizeof(ident)) != 0) {
        printf("ident and result where different\n");
    } else {
        printf("ok\n");
    }
}

void test_gt_elf32_get_header(void)
{
    // arrange
    gt_elf32_ehdr* result = NULL;
    gt_elf32_ehdr  ehdr = { 0 };
    gt_buf         buf = { .data = &ehdr, .size = sizeof(ehdr) };
    uint8_t        magic[4] = { 0x7f, 'E', 'L', 'F' };

    ehdr.ehsize = sizeof(ehdr);
    ehdr.machine = 1;
    ehdr.entry = 1;
    memcpy(ehdr.ident.magic, magic, sizeof(magic));
    ehdr.ident.clazz = GT_ELF_IDENT_CLASS_32;

    // act
    int code = gt_elf32_get_header(&buf, &result);

    // assert
    if (code) {
        printf("code was %i\n", code);
    } else if (memcmp(&ehdr, result, sizeof(ehdr)) != 0) {
        printf("ident and result where different\n");
    } else {
        printf("ok\n");
    }
}

void test_gt_elf32_get_shdr_array(void)
{
    // arrange
    struct {
        gt_elf32_ehdr ehdr;
        gt_elf32_shdr shdr[1];
    } elf = { 0 };

    gt_buf  buf = { .data = &elf, .size = sizeof(elf) };
    uint8_t magic[4] = { 0x7f, 'E', 'L', 'F' };

    elf.ehdr.ehsize = sizeof(elf.ehdr);
    elf.ehdr.machine = 1;
    elf.ehdr.entry = 1;
    memcpy(elf.ehdr.ident.magic, magic, sizeof(magic));
    elf.ehdr.ident.clazz = GT_ELF_IDENT_CLASS_32;
    elf.ehdr.shentsize = sizeof(elf.shdr[0]);
    elf.ehdr.shoff = (void*)&elf.shdr[0] - (void*)&elf.ehdr;
    elf.ehdr.shnum = 1;

    // act
    gt_elf32_shdr* shdr = NULL;
    size_t         num = 0;
    int            code = gt_elf32_get_shdr_array(&buf, &num, &shdr);

    // assert
    if (code) {
        gt_trace_point tp = { 0 };
        gt_trace_get_level(0, &tp);
        printf("code was %i\n", code);
        printf("  file: %s\n", tp.location.file);
        printf("  line: %i\n", tp.location.line);
    } else if (shdr == NULL) {
        printf("shdr was NULL\n");
    } else if (num == 0) {
        printf("num was zero\n");
    } else if (shdr != &elf.shdr[0]) {
        printf("shdr was %p; expected %p\n", shdr, &elf.shdr[0]);
    } else {
        printf("ok\n");
    }
}

void test_gt_elf32_get_phdr_array(void)
{
    // arrange
    struct {
        gt_elf32_ehdr ehdr;
        gt_elf32_phdr phdr[1];
    } elf = { 0 };

    gt_buf buf = { .data = &elf, .size = sizeof(elf) };

    elf.ehdr.ehsize = sizeof(elf.ehdr);
    elf.ehdr.machine = 1;
    elf.ehdr.entry = 1;
    memcpy(elf.ehdr.ident.magic, ELF_RO_MAGIC, sizeof(ELF_RO_MAGIC));
    elf.ehdr.ident.clazz = GT_ELF_IDENT_CLASS_32;
    elf.ehdr.phentsize = sizeof(elf.phdr[0]);
    elf.ehdr.phoff = (void*)&elf.phdr[0] - (void*)&elf.ehdr;
    elf.ehdr.phnum = 1;

    // act
    gt_elf32_phdr* phdr = NULL;
    size_t         num = 0;
    int            code = gt_elf32_get_phdr_array(&buf, &num, &phdr);

    // assert
    if (code) {
        gt_trace_point tp = { 0 };
        gt_trace_get_level(0, &tp);
        printf("code was %i\n", code);
        printf("  file: %s\n", tp.location.file);
        printf("  line: %i\n", tp.location.line);
    } else if (phdr == NULL) {
        printf("phdr was NULL\n");
    } else if (num == 0) {
        printf("num was zero\n");
    } else if (phdr != &elf.phdr[0]) {
        printf("shdr was %p; expected %p\n", phdr, &elf.phdr[0]);
    } else {
        printf("ok\n");
    }
}

void test_gt_elf32_get_sym_array(void)
{
}

int main(int argc, char** argv)
{
    test_gt_elf_get_ident();
    test_gt_elf32_get_header();
    test_gt_elf32_get_shdr_array();
    test_gt_elf32_get_phdr_array();
    return 0;
}
