/*
 *  Copyright (c) 2026 Gaël Fortier <gael.fortier.1@ens.etsmtl.ca>
 */

#include "elf.h"
#include <string.h>

/****************************
 *    Internal functions
 ****************************/

/****************************
 * Internal 32 bit functions
 ****************************/

static int gt_elf32_hash_name(gt_elf32_ehdr* ehdr, const char* name, size_t max, unsigned int* out)
{
    GT_THROWIF(ehdr == NULL, GT_ELF_INVALID_EHDR);
    GT_THROWIF(name == NULL, GT_ELF_INVALID_NAME);
    GT_THROWIF(out == NULL, GT_ELF_INVALID_OUT);

    unsigned int sum = 0;
    for (int i = 0; name[i] != '\0' && i < max; i++) {
        sum += name[i];
        if (sum >= ehdr->shnum) {
            sum -= ehdr->shnum;
        }
    }

    *out = sum;
    return GT_ELF_OK;
}

static int gt_elf32_lookup_insert(gt_elf_shlookup* lookup, unsigned int index, gt_elf32_shdr* shdr)
{
    GT_THROWIF(lookup == NULL, GT_ELF_INVALID_LOOKUP);
    GT_THROWIF(index >= lookup->length, GT_ELF_INVALID_SIZE);

    // infinite loop very unlikely to happen.
    while (lookup->lookup[index] != NULL) {
        index++;
        if (index >= lookup->length)
            index -= lookup->length;
    }

    lookup->lookup[index] = shdr;
    return GT_ELF_OK;
}

/*************************
 *    32 bit functions
 *************************/

int gt_elf32_load(gt_buf* buf, gt_elf32* out)
{
    GT_THROWIF(out == NULL, GT_ELF_INVALID_OUT);
    GT_THROWIF(out->buffer != NULL, GT_ELF_FAILURE_LOADED);
    GT_THROWIF(out->header != NULL, GT_ELF_FAILURE_LOADED);
    GT_THROWIF(out->programs != NULL, GT_ELF_FAILURE_LOADED);
    GT_THROWIF(out->sections != NULL, GT_ELF_FAILURE_LOADED);

    gt_elf_ident* ident = NULL;
    GT_TRYTHIS(gt_buf_effective(buf, 0, sizeof(*ident), (void**)&ident));
    GT_THROWIF(ident->clazz != GT_ELF_IDENT_CLASS_32, GT_ELF_INVALID_CLASS);

    gt_elf32_ehdr* header = NULL;
    GT_TRYTHIS(gt_buf_effective(buf, 0, sizeof(*header), (void**)&header));

    gt_elf32_shdr* sections = NULL;
    GT_TRYTHIS(gt_buf_effective(buf, header->shoff, header->shentsize * header->shnum, (void**)&sections));

    gt_elf32_phdr* programs = NULL;
    GT_TRYTHIS(gt_buf_effective(buf, header->phoff, header->phentsize * header->phnum, (void**)&programs));

    if (header->shstrndx == GT_ELF_SHN_UNDEF) {
        out->buffer = buf;
        out->header = header;
        out->programs = programs;
        out->sections = sections;
        out->shstrtab = (gt_elf_strtab) { 0 };
        out->shlookup = (gt_elf_shlookup) { 0 };
    }

    gt_elf32_shdr*  shstrsh = &sections[header->shstrndx];
    gt_elf_shlookup shlookup = { 0 };
    gt_elf_strtab   strtab = { 0 };

    shlookup.length = header->shnum;
    shlookup.lookup = malloc(sizeof(void**) * header->shnum);
    GT_THROWIF(shlookup.lookup == NULL, GT_ELF_FAILURE_MALLOC);

    strtab.size = shstrsh->size;
    strtab.addr = NULL;
    GT_TRYSAFE(gt_buf_effective(buf, shstrsh->offset, shstrsh->size, (void**)&strtab.addr), goto on_error);

    memset(shlookup.lookup, 0, sizeof(void**) * header->shnum);

    for (uint64_t i = 0; i < header->shnum; i++) {
        unsigned int index = 0;
        const char*  name = strtab.addr + sections[i].name;
        GT_TRYSAFE(gt_elf32_hash_name(header, name, strtab.size, &index), goto on_error);
        GT_TRYSAFE(gt_elf32_lookup_insert(&shlookup, index, &sections[i]), goto on_error);
    }

    out->buffer = buf;
    out->header = header;
    out->programs = programs;
    out->sections = sections;
    out->shstrtab = strtab;
    out->shlookup = shlookup;
    return GT_ELF_OK;

on_error:
    free(shlookup.lookup);
    return gt_trace_get_result();
}

int gt_elf32_unload(gt_elf32* elf)
{
    GT_THROWIF(elf == NULL, GT_ELF_INVALID_ELF);

    memset(elf, 0, sizeof(*elf));
    return GT_ELF_OK;
}

int gt_elf32_shnum(gt_elf32* elf, size_t* out)
{
    int loaded = 0;

    GT_TRYTHIS(gt_elf32_loaded(elf, &loaded));
    GT_THROWIF(loaded == GT_ELF_UNLOADED, GT_ELF_FAILURE_UNLOADED);
    GT_THROWIF(out == NULL, GT_ELF_INVALID_OUT);

    *out = elf->header->shnum;
    return GT_ELF_OK;
}
