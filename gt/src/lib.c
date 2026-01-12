/*
    Copyright (c) 2025 Gaël Fortier <gael.fortier.1@ens.etsmtl.ca>
*/

#include "lib.h"
#include <elf.h>
#include <stdint.h>
#include <time.h>

static struct {
    elf_trace trace;
    char path[PATH_MAX];
    uint8_t* data;
    size_t size;
} elfpv = { 0 };

// static void elf_dump_trace(void)
// {
//     char dump_filename[20] = { 0 };
//     snprintf(dump_filename, sizeof(dump_filename), "%li", elfpv.trace.timestamp);
//
//     FILE* dump = fopen(dump_filename, "a+");
//     elf_print_to_file_error_trace(dump);
//     fclose(dump);
//
//     elfpv.trace.length = 0;
//     memset(elfpv.trace.trace, 0, sizeof(elfpv.trace.trace));
// }

int elf_reset_index_iterator(elf_index_iterator* iterator_ref)
{
    if (iterator_ref == NULL) {
        return elf_stack_error(ELF_ERR_NULL);
    }

    iterator_ref->object = NULL;
    iterator_ref->index = 0;
    iterator_ref->done = 0;
    return elf_stack_error(ELF_OK);
}

int elf_stack_error_struct(elf_error error)
{
    if (elfpv.trace.old) {
        elfpv.trace.length = 0;
        elfpv.trace.old = 0;
        time(&elfpv.trace.timestamp);
    }

    if (elfpv.trace.length < ELF_TRACE_SIZE) {
        size_t* i = &elfpv.trace.length;
        elfpv.trace.trace[*i] = error;
        (*i)++;
        (*i) = (*i) % ELF_TRACE_SIZE;
    }

    return error.code;
}

int elf_has_error(void)
{
    for (size_t i = 0; i < elfpv.trace.length; i++) {
        if (elfpv.trace.trace[i].code != ELF_OK)
            return 1;
    }

    return 0;
}

void elf_reset_error_trace(void)
{
    elfpv.trace.old = 1;
}

void elf_print_to_file_error_trace(FILE* output)
{
    if (output == NULL) {
        return;
    }

    for (size_t i = 0; i < elfpv.trace.length; i++) {
        fprintf(output, "%zu: %i\n", i, elfpv.trace.trace[i].code);
        fprintf(output, "\tfile: %s\n", elfpv.trace.trace[i].file);
        fprintf(output, "\tfunc: %s\n", elfpv.trace.trace[i].func);
        fprintf(output, "\tline: %i\n\n", elfpv.trace.trace[i].line);
    }
}

int elf_elf_loaded(void)
{
    return elfpv.data != NULL;
}

int elf_load_elf(const char* path)
{
    if (elf_elf_loaded()) {
        return elf_stack_error(ELF_ERR_LOADED);
    }

    FILE* file = fopen(path, "rb");
    if (file == NULL) {
        return elf_stack_error(ELF_ERR_PATH);
    }

    fseek(file, 0, SEEK_END);
    elfpv.size = ftell(file);
    rewind(file);

    elfpv.data = malloc(elfpv.size);
    if (elfpv.data == NULL) {
        fclose(file);
        return elf_stack_error(ELF_ERR_MALLOC);
    }

    strncpy(elfpv.path, path, PATH_MAX);
    fread(elfpv.data, 1, elfpv.size, file);
    fclose(file);
    return elf_stack_error(ELF_OK);
}

int elf_unload_elf(void)
{
    if (!elf_elf_loaded()) {
        return elf_stack_error(ELF_ERR_UNLOADED);
    }

    free(elfpv.data);
    elfpv.data = NULL;
    strcpy(elfpv.path, "");
    elfpv.size = 0;
    return elf_stack_error(ELF_OK);
}

int elf_print_elf(void)
{
    elf_newl("%s", "");
    size_t path_len = strlen(elfpv.path);
    elf_snewl(elfpv.path, path_len);
    elf_unewl(elfpv.size);
    return elf_stack_error(ELF_OK);
}

int elf_save_elf(const char* path)
{
    FILE* file = fopen("out", "wb");
    if (file == NULL) {
        return elf_stack_error(ELF_ERR_NULL);
    }

    fwrite(elfpv.data, 1, elfpv.size, file);
    fclose(file);
    return elf_stack_error(ELF_OK);
}

int elf_offset(size_t offset, size_t ref_size, void** ref_ref)
{
    if (!elf_elf_loaded()) {
        return elf_stack_error(ELF_ERR_UNLOADED);
    }

    if (offset + ref_size > elfpv.size) {
        printf("%zu + %zu > %zu\n", offset, ref_size, elfpv.size);
        return elf_stack_error(ELF_ERR_SEGFAULT);
    }

    *ref_ref = elfpv.data + offset;
    return elf_stack_error(ELF_OK);
}

int elf_get_offset(const void* end, const void* start, size_t* offset_ref)
{
    if (!elf_elf_loaded()) {
        return elf_stack_error(ELF_ERR_UNLOADED);
    }

    void* absolute_end = elfpv.data + elfpv.size;
    void* absolute_start = elfpv.data;

    if (end > absolute_end || end < absolute_start) {
        return elf_stack_error(ELF_ERR_SEGFAULT);
    }

    if (start > absolute_end || start < absolute_start) {
        return elf_stack_error(ELF_ERR_SEGFAULT);
    }

    *offset_ref = end - start;
    return elf_stack_error(ELF_OK);
}

int elf_get_hdr(Elf64_Ehdr** hdr_ref)
{
    elf_check(elf_offset(0, sizeof(**hdr_ref), (void**)hdr_ref));
    return elf_stack_error(ELF_OK);
}

int elf_print_header(void)
{
    Elf64_Ehdr* hdr;
    elf_check(elf_get_hdr(&hdr));

    elf_bnewl(hdr->e_ident, EI_NIDENT);
    elf_unewl(hdr->e_type);
    elf_unewl(hdr->e_machine);
    elf_unewl(hdr->e_version);
    elf_unewl(hdr->e_entry);
    elf_unewl(hdr->e_phoff);
    elf_unewl(hdr->e_shoff);
    elf_unewl(hdr->e_flags);
    elf_unewl(hdr->e_ehsize);
    elf_unewl(hdr->e_phentsize);
    elf_unewl(hdr->e_phnum);
    elf_unewl(hdr->e_shentsize);
    elf_unewl(hdr->e_shnum);
    elf_unewl(hdr->e_shstrndx);

    return elf_stack_error(ELF_OK);
}

int elf_get_phdr_windex(size_t index, Elf64_Phdr** phdr_ref)
{
    Elf64_Ehdr* hdr;
    elf_check(elf_get_hdr(&hdr));

    if (index >= hdr->e_phnum) {
        return elf_stack_error(ELF_ERR_INDEX);
    }

    size_t offset = hdr->e_phoff + hdr->e_phentsize * index;
    elf_check(elf_offset(offset, sizeof(**phdr_ref), (void**)phdr_ref));
    return elf_stack_error(ELF_OK);
}

int elf_get_phdr_wtype(Elf64_Word type, Elf64_Phdr** phdr_ref)
{
    Elf64_Ehdr* hdr;
    elf_check(elf_get_hdr(&hdr));

    for (size_t i = 0; i < hdr->e_phnum; i++) {
        Elf64_Phdr* phdr;
        elf_check(elf_get_phdr_windex(i, &phdr));

        if (phdr->p_type == type) {
            *phdr_ref = phdr;
            return elf_stack_error(ELF_OK);
        }
    }

    return elf_stack_error(ELF_ERR_NOT_FOUND);
}

int elf_print_phdr(Elf64_Phdr* phdr)
{
    const char* typestr[] = {
        "PT_NULL",
        "PT_LOAD",
        "PT_DYNAMIC",
        "PT_INTERP",
        "PT_NOTE",
        "PT_SHLIB",
        "PT_PHDR",
        "PT_TLS",
    };

    elf_newl("%s", "");
    if (phdr->p_type < PT_NUM) {
        const char* phdr_p_type = typestr[phdr->p_type];
        elf_snewl(phdr_p_type, strlen(phdr_p_type));
    }
    elf_unewl(phdr->p_type);
    elf_unewl(phdr->p_flags);
    elf_unewl(phdr->p_offset);
    elf_unewl(phdr->p_vaddr);
    elf_unewl(phdr->p_paddr);
    elf_unewl(phdr->p_filesz);
    elf_unewl(phdr->p_memsz);
    elf_unewl(phdr->p_align);
    return elf_stack_error(ELF_OK);
}

int elf_get_shdr_strtab_shdr(Elf64_Shdr** shdr_ref)
{
    Elf64_Ehdr* hdr;
    elf_check(elf_get_hdr(&hdr));

    Elf64_Shdr* shtext_sh;
    elf_check(elf_get_shdr_windex(hdr->e_shstrndx, &shtext_sh));

    *shdr_ref = shtext_sh;
    return elf_stack_error(ELF_OK);
}

int elf_get_shdr_windex(size_t index, Elf64_Shdr** shdr_ref)
{
    Elf64_Ehdr* hdr;
    elf_check(elf_get_hdr(&hdr));

    if (index >= hdr->e_shnum) {
        return elf_stack_error(ELF_ERR_INDEX);
    }

    size_t offset = hdr->e_shoff + hdr->e_shentsize * index;
    elf_check(elf_offset(offset, sizeof(**shdr_ref), (void**)shdr_ref));
    return elf_stack_error(ELF_OK);
}

int elf_get_shdr_wname(const char* name, Elf64_Shdr** shdr_ref)
{
    Elf64_Ehdr* hdr;
    elf_check(elf_get_hdr(&hdr));

    Elf64_Shdr* shtext_sh;
    elf_check(elf_get_shdr_strtab_shdr(&shtext_sh));

    for (size_t i = 0; i < hdr->e_shnum; i++) {
        Elf64_Shdr* sh;
        elf_check(elf_get_shdr_windex(i, &sh));

        const char* sh_text;
        elf_check(elf_get_strtab_shdr_text(shtext_sh, sh->sh_name, &sh_text));

        if (strcmp(name, sh_text) == 0) {
            *shdr_ref = sh;
            return elf_stack_error(ELF_OK);
        }
    }

    return elf_stack_error(ELF_ERR_NOT_FOUND);
}

int elf_get_shdr_wtype(Elf64_Word type, Elf64_Shdr** shdr_ref)
{
    Elf64_Ehdr* hdr;
    elf_check(elf_get_hdr(&hdr));

    for (size_t i = 0; i < hdr->e_shnum; i++) {
        Elf64_Shdr* sh;
        elf_check(elf_get_shdr_windex(i, &sh));

        if (sh->sh_type == type) {
            *shdr_ref = sh;
            return elf_stack_error(ELF_OK);
        }
    }

    return elf_stack_error(ELF_ERR_NOT_FOUND);
}

int elf_print_shdr(Elf64_Shdr* shdr)
{
    Elf64_Ehdr* hdr;
    elf_check(elf_get_hdr(&hdr));

    Elf64_Shdr* shtext_sh;
    elf_check(elf_get_shdr_strtab_shdr(&shtext_sh));

    Elf64_Shdr* first_sh;
    elf_check(elf_get_shdr_windex(0, &first_sh));
    size_t shdr_index = (shdr - first_sh);

    const char* shtext;
    elf_check(elf_get_strtab_shdr_text(shtext_sh, shdr->sh_name, &shtext));
    size_t shtext_len = strlen(shtext);

    elf_newl("%s", "");
    elf_unewl(shdr_index);
    elf_snewl(shtext, shtext_len);
    elf_unewl(shdr->sh_name);
    elf_unewl(shdr->sh_type);
    elf_unewl(shdr->sh_flags);
    elf_unewl(shdr->sh_offset);
    elf_unewl(shdr->sh_size);
    elf_unewl(shdr->sh_link);
    elf_unewl(shdr->sh_info);
    elf_unewl(shdr->sh_addralign);
    elf_unewl(shdr->sh_entsize);

    return elf_stack_error(ELF_OK);
}

int elf_strtab_shdr_type(Elf64_Shdr* strtab_sh)
{
    int result = strtab_sh->sh_type == SHT_STRTAB;
    return elf_stack_error((result) ? ELF_OK : ELF_ERR_TYPE);
}

int elf_get_strtab_shdr_text(Elf64_Shdr* strtab_sh, size_t index, const char** text_ref)
{
    elf_check(elf_strtab_shdr_type(strtab_sh));
    size_t offset = strtab_sh->sh_offset + index;
    elf_check(elf_offset(offset, sizeof(**text_ref), (void**)text_ref));
    return elf_stack_error(ELF_OK);
}

int elf_print_strtab_shdr_text(Elf64_Shdr* strtab_sh)
{
    elf_check(elf_strtab_shdr_type(strtab_sh));
    elf_newl("%s", "");
    for (size_t i = 1; i < strtab_sh->sh_size; i++) {
        const char* str;
        elf_check(elf_get_strtab_shdr_text(strtab_sh, i, &str));
        size_t str_len = strlen(str);
        i += str_len;
        elf_snewl(str, str_len);
    }
    return elf_stack_error(ELF_OK);
}

int elf_sym_shdr_type(Elf64_Shdr* sym_sh)
{
    int result = sym_sh->sh_type == SHT_SYMTAB;
    return elf_stack_error((result) ? ELF_OK : ELF_ERR_TYPE);
}

int elf_sym_type(Elf64_Sym* sym, uint16_t type)
{
    int result = (ELF64_ST_TYPE(sym->st_info) == type);
    return elf_stack_error((result) ? ELF_OK : ELF_ERR_TYPE);
}

int elf_get_sym_strtab_shdr(Elf64_Shdr* sym_sh, Elf64_Shdr** shdr_ref)
{
    elf_check(elf_sym_shdr_type(sym_sh));

    Elf64_Shdr* strtab_sh;
    elf_check(elf_get_shdr_windex(sym_sh->sh_link, &strtab_sh));
    elf_check(elf_strtab_shdr_type(strtab_sh));

    *shdr_ref = strtab_sh;
    return elf_stack_error(ELF_OK);
}

int elf_get_sym_num(Elf64_Shdr* sym_sh, size_t* num_ref)
{
    elf_check(elf_sym_shdr_type(sym_sh));

    if (sym_sh->sh_entsize == 0) {
        return elf_stack_error(ELF_ERR_DIV_ZERO);
    }

    size_t sym_num = sym_sh->sh_size / sym_sh->sh_entsize;
    *num_ref = sym_num;
    return elf_stack_error(ELF_OK);
}

int elf_get_sym_shdr(Elf64_Shdr** sym_sh)
{
    elf_check(elf_get_shdr_wname(".symtab", sym_sh));
    return elf_stack_error(ELF_OK);
}

int elf_get_sym_windex(Elf64_Shdr* sym_sh, size_t index, Elf64_Sym** sym_ref)
{
    elf_check(elf_sym_shdr_type(sym_sh));

    size_t sym_num;
    elf_check(elf_get_sym_num(sym_sh, &sym_num));

    if (index >= sym_num) {
        elf_stack_error(ELF_ERR_INDEX);
    }

    size_t offset = sym_sh->sh_offset + index * sym_sh->sh_entsize;
    Elf64_Sym* sym;
    elf_check(elf_offset(offset, sym_sh->sh_entsize, (void**)&sym));

    *sym_ref = sym;
    return elf_stack_error(ELF_OK);
}

int elf_get_sym_wname(Elf64_Shdr* sym_sh, const char* name, Elf64_Sym** sym_ref)
{
    elf_check(elf_sym_shdr_type(sym_sh));

    Elf64_Shdr* strtab_sh;
    elf_check(elf_get_sym_strtab_shdr(sym_sh, &strtab_sh));

    size_t sym_num;
    elf_check(elf_get_sym_num(sym_sh, &sym_num));

    for (size_t i = 0; i < sym_num; i++) {
        size_t offset = sym_sh->sh_offset + i * sym_sh->sh_entsize;
        Elf64_Sym* sym;
        elf_check(elf_offset(offset, sym_sh->sh_entsize, (void**)&sym));

        const char* sym_name;
        elf_check(elf_get_strtab_shdr_text(strtab_sh, sym->st_name, &sym_name));

        if (strcmp(sym_name, name) == 0) {
            *sym_ref = sym;
            return elf_stack_error(ELF_OK);
        }
    }

    return elf_stack_error(ELF_ERR_NOT_FOUND);
}

int elf_get_sym_wtype(Elf64_Shdr* sym_sh, uint16_t type, elf_index_iterator* iterator_ref)
{
    elf_check(elf_sym_shdr_type(sym_sh));

    Elf64_Shdr* strtab_sh;
    elf_check(elf_get_sym_strtab_shdr(sym_sh, &strtab_sh));

    size_t sym_num;
    elf_check(elf_get_sym_num(sym_sh, &sym_num));

    size_t index = (iterator_ref->index) ? iterator_ref->index + 1 : 0;
    for (int64_t i = index; i < sym_num; i++) {
        size_t offset = sym_sh->sh_offset + i * sym_sh->sh_entsize;
        Elf64_Sym* sym;

        elf_check(elf_offset(offset, sym_sh->sh_entsize, (void**)&sym));

        uint16_t sym_type = ELF64_ST_TYPE(sym->st_info);
        if (sym_type == type) {
            iterator_ref->object = sym;
            iterator_ref->index = i;
            iterator_ref->done = (i == sym_num - 1);
            return elf_stack_error(ELF_OK);
        }
    }

    iterator_ref->object = NULL;
    iterator_ref->done = 1;
    return elf_stack_error(ELF_DONE);
}

int elf_get_sym_offset(Elf64_Sym* sym, size_t* offset_ref)
{
    int result0 = elf_sym_type(sym, STT_FUNC);
    int result1 = elf_sym_type(sym, STT_OBJECT);
    if (result0 != ELF_OK && result1 != ELF_OK) {
        return elf_stack_error(ELF_ERR_TYPE);
    }

    Elf64_Ehdr* hdr;
    elf_check(elf_get_hdr(&hdr));

    switch (hdr->e_type) {
    case ET_REL:
    case ET_EXEC:
    case ET_DYN:
        break;
    default:
        return elf_stack_error(ELF_ERR_ELF);
    }

    if (sym->st_shndx == SHN_COMMON) {
        return elf_stack_error(ELF_ERR_INDEX);
    }

    size_t offset;

    if (hdr->e_type == ET_EXEC || hdr->e_type == ET_DYN) {
        Elf64_Phdr* phdr;
        elf_check(elf_get_phdr_wtype(PT_LOAD, &phdr));
        offset = sym->st_value - phdr->p_vaddr + phdr->p_offset;
    }

    if (hdr->e_type == ET_REL) {
        Elf64_Shdr* section;
        elf_check(elf_get_shdr_windex(sym->st_shndx, &section));
        offset = sym->st_value + section->sh_offset;
    }

    *offset_ref = offset;
    return elf_stack_error(ELF_OK);
}

static int elf_get_writable_sym_bytes(Elf64_Sym* sym, elf_bytebuffer* buf_ref, size_t* size)
{
    size_t offset;
    elf_check(elf_get_sym_offset(sym, &offset));
    elf_check(elf_offset(offset, sym->st_size, (void**)buf_ref));
    *size = sym->st_size;
    return elf_stack_error(ELF_OK);
}

int elf_get_readonly_sym_bytes(Elf64_Sym* sym, elf_ro_bytebuff* buf_ref, size_t* size)
{
    elf_bytebuffer buffer;
    elf_check(elf_get_writable_sym_bytes(sym, &buffer, size));
    *buf_ref = buffer;
    return elf_stack_error(ELF_OK);
}

int elf_set_sym_bytes(Elf64_Sym* sym, size_t offset, elf_bytebuffer buf, size_t size)
{
    size_t buf_len;
    elf_bytebuffer sym_buf;
    elf_check(elf_get_writable_sym_bytes(sym, &sym_buf, &buf_len));

    void* addr;
    elf_check(elf_offset(offset, size, &addr));

    memcpy(sym_buf + offset, buf, size);
    return elf_stack_error(ELF_OK);
}

int elf_print_sym_bytes_2(Elf64_Sym* sym)
{
    elf_ro_bytebuff buffer;
    size_t size;
    elf_check(elf_get_readonly_sym_bytes(sym, &buffer, &size));
    elf_check(elf_print_sym_bytes(sym, buffer, size));
    return elf_stack_error(ELF_OK);
}

int elf_print_sym_bytes(Elf64_Sym* func_sym, elf_ro_bytebuff bytes, size_t size)
{
    Elf64_Ehdr* hdr;
    elf_check(elf_get_hdr(&hdr));

    size_t offset;
    elf_check(elf_get_offset(bytes, hdr, &offset));

    const size_t row_length = 16;

    elf_pnewl((void*)offset);
    elf_unewl((size_t)offset);
    elf_pnewl((void*)size);
    elf_unewl((size_t)size);

    uint8_t* b = elfpv.data + offset;

    printf("\n%-12i ", 0);
    for (size_t i = 0; i < row_length; i++) {
        printf("%02zX ", i);
    }

    fputc('\n', stdout);

    for (size_t i = 0; i < size; i++) {
        (i % row_length == 0)
            ? printf("\n%-12zX %02hhX ", i, b[i])
            : printf("%02hhX ", b[i]);
    }

    fputc('\n', stdout);
    return elf_stack_error(ELF_OK);
}

int elf_print_sym(Elf64_Shdr* sym_sh, Elf64_Sym* sym)
{
    Elf64_Shdr* strtab_sh;
    elf_check(elf_get_sym_strtab_shdr(sym_sh, &strtab_sh));

    const char* sym_name;
    if (elf_get_strtab_shdr_text(strtab_sh, sym->st_name, &sym_name) != ELF_OK) {
        sym_name = "ERROR";
    }

    size_t name_length = strlen(sym_name);

    unsigned sym_type = ELF64_ST_TYPE(sym->st_info);
    unsigned sym_bind = ELF64_ST_BIND(sym->st_info);
    unsigned sym_visb = ELF64_ST_VISIBILITY(sym->st_other);

    elf_newl("%s", "");
    elf_snewl(sym_name, name_length);
    elf_unewl(sym->st_name);
    elf_unewl(sym->st_info);
    elf_unewl(sym_type);
    elf_unewl(sym_bind);
    elf_unewl(sym->st_other);
    elf_unewl(sym_visb);
    elf_unewl(sym->st_shndx);
    elf_unewl(sym->st_value);
    elf_unewl(sym->st_size);

    return elf_stack_error(ELF_OK);
}

int elf_print_sym_shdr(Elf64_Shdr* sym_sh)
{
    elf_check(elf_sym_shdr_type(sym_sh));

    size_t sym_num;
    elf_check(elf_get_sym_num(sym_sh, &sym_num));

    for (size_t i = 0; i < sym_num; i++) {
        Elf64_Sym* sym;
        elf_check(elf_get_sym_windex(sym_sh, i, &sym));
        elf_check(elf_print_sym(sym_sh, sym));
    }

    return elf_stack_error(ELF_OK);
}

void elf_impl_bnewl(const char* expr, uint8_t* buff, size_t size)
{
    size_t expr_len = strlen(expr);
    elf_newl("%-30s %-12zu %-12s", expr, size, "...");
    for (size_t i = 0; i < size; i++) {
        int int_buf_size = 22;
        char int_buf[int_buf_size];
        char name_buf[sizeof(expr_len) + int_buf_size];
        memset(name_buf, 0, sizeof(name_buf));
        memset(int_buf, 0, sizeof(int_buf));

        snprintf(int_buf, sizeof(int_buf), "%zu", i);
        strcat(name_buf, expr);
        strcat(name_buf, "@");
        strcat(name_buf, int_buf);
        elf_gnewl("%-12hhx", name_buf, (size_t)1, buff[i]);
    }
}
