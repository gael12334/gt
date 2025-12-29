/*
    Copyright (c) 2025 Gaël Fortier <gael.fortier.1@ens.etsmtl.ca>
*/

#include "elfpv_lib.h"
#include <elf.h>
#include <stdint.h>

static struct {
    elfpv_trace trace;
    char path[PATH_MAX];
    uint8_t* data;
    size_t size;
} elfpv = { 0 };

int elfpv_stack_error_struct(elfpv_error error)
{
    if (elfpv.trace.old) {
        elfpv.trace.length = 0;
        elfpv.trace.old = 0;
    }

    if (elfpv.trace.length < ELFPV_TRACE_SIZE) {
        size_t* i = &elfpv.trace.length;
        elfpv.trace.trace[*i] = error;
        (*i)++;
    }

    return error.code;
}

void elfpv_reset_error_trace(void)
{
    elfpv.trace.old = 1;
}

void elfpv_print_error_trace(void)
{
    for (size_t i = 0; i < elfpv.trace.length; i++) {
        printf("%zu: %i\n", i, elfpv.trace.trace[i].code);
        printf("\tfile: %s\n", elfpv.trace.trace[i].file);
        printf("\tfunc: %s\n", elfpv.trace.trace[i].func);
        printf("\tline: %i\n\n", elfpv.trace.trace[i].line);
    }
}

int elfpv_elf_loaded(void)
{
    return elfpv.data != NULL;
}

int elfpv_load_elf(const char* path)
{
    if (elfpv_elf_loaded()) {
        return elfpv_stack_error(ELFPV_ERR_LOADED);
    }

    FILE* file = fopen(path, "r");
    if (file == NULL) {
        return elfpv_stack_error(ELFPV_ERR_PATH);
    }

    fseek(file, 0, SEEK_END);
    elfpv.size = ftell(file);
    rewind(file);

    elfpv.data = malloc(elfpv.size);
    if (elfpv.data == NULL) {
        fclose(file);
        return elfpv_stack_error(ELFPV_ERR_MALLOC);
    }

    strncpy(elfpv.path, path, PATH_MAX);
    fread(elfpv.data, 1, elfpv.size, file);
    fclose(file);
    return elfpv_stack_error(ELFPV_OK);
}

int elfpv_unload_elf(void)
{
    if (!elfpv_elf_loaded()) {
        return elfpv_stack_error(ELFPV_ERR_UNLOADED);
    }

    free(elfpv.data);
    strcpy(elfpv.path, "");
    elfpv.size = 0;
    return elfpv_stack_error(ELFPV_OK);
}

int elfpv_print_elf(void)
{
    elfpv_newl("%s", "");
    size_t path_len = strlen(elfpv.path);
    elfpv_snewl(elfpv.path, path_len);
    elfpv_unewl(elfpv.size);
    return elfpv_stack_error(ELFPV_OK);
}

int elfpv_save_elf(const char* path)
{
    return elfpv_stack_error(ELFPV_OK);
}

int elfpv_offset(size_t offset, size_t ref_size, void** ref_ref)
{
    if (!elfpv_elf_loaded()) {
        return elfpv_stack_error(ELFPV_ERR_UNLOADED);
    }

    if (offset + ref_size > elfpv.size) {
        return elfpv_stack_error(ELFPV_ERR_SEGFAULT);
    }

    *ref_ref = elfpv.data + offset;
    return elfpv_stack_error(ELFPV_OK);
}

int elfpv_get_hdr(Elf64_Ehdr** hdr_ref)
{
    elfpv_check(elfpv_offset(0, sizeof(**hdr_ref), (void**)hdr_ref));
    return elfpv_stack_error(ELFPV_OK);
}

int elfpv_print_header(void)
{
    Elf64_Ehdr* hdr;
    elfpv_check(elfpv_get_hdr(&hdr));

    elfpv_bnewl(hdr->e_ident, EI_NIDENT);
    elfpv_unewl(hdr->e_type);
    elfpv_unewl(hdr->e_machine);
    elfpv_unewl(hdr->e_version);
    elfpv_unewl(hdr->e_entry);
    elfpv_unewl(hdr->e_phoff);
    elfpv_unewl(hdr->e_shoff);
    elfpv_unewl(hdr->e_flags);
    elfpv_unewl(hdr->e_ehsize);
    elfpv_unewl(hdr->e_phentsize);
    elfpv_unewl(hdr->e_phnum);
    elfpv_unewl(hdr->e_shentsize);
    elfpv_unewl(hdr->e_shnum);
    elfpv_unewl(hdr->e_shstrndx);

    return elfpv_stack_error(ELFPV_OK);
}

int elfpv_get_phdr_windex(size_t index, Elf64_Phdr** phdr_ref)
{
    Elf64_Ehdr* hdr;
    elfpv_check(elfpv_get_hdr(&hdr));

    if (index >= hdr->e_phnum) {
        return elfpv_stack_error(ELFPV_ERR_INDEX);
    }

    size_t offset = hdr->e_phoff + hdr->e_phentsize * index;
    elfpv_check(elfpv_offset(offset, sizeof(**phdr_ref), (void**)phdr_ref));
    return elfpv_stack_error(ELFPV_OK);
}

int elfpv_print_phdr(Elf64_Phdr* phdr)
{
    elfpv_newl("%s", "");
    elfpv_unewl(phdr->p_type);
    elfpv_unewl(phdr->p_flags);
    elfpv_unewl(phdr->p_offset);
    elfpv_unewl(phdr->p_vaddr);
    elfpv_unewl(phdr->p_paddr);
    elfpv_unewl(phdr->p_filesz);
    elfpv_unewl(phdr->p_memsz);
    elfpv_unewl(phdr->p_align);
    return elfpv_stack_error(ELFPV_OK);
}

int elfpv_get_shdr_strtab_shdr(Elf64_Shdr** shdr_ref)
{
    Elf64_Ehdr* hdr;
    elfpv_check(elfpv_get_hdr(&hdr));

    Elf64_Shdr* shtext_sh;
    elfpv_check(elfpv_get_shdr_windex(hdr->e_shstrndx, &shtext_sh));

    *shdr_ref = shtext_sh;
    return elfpv_stack_error(ELFPV_OK);
}

int elfpv_get_shdr_windex(size_t index, Elf64_Shdr** shdr_ref)
{
    Elf64_Ehdr* hdr;
    elfpv_check(elfpv_get_hdr(&hdr));

    if (index >= hdr->e_shnum) {
        return elfpv_stack_error(ELFPV_ERR_INDEX);
    }

    size_t offset = hdr->e_shoff + hdr->e_shentsize * index;
    elfpv_check(elfpv_offset(offset, sizeof(**shdr_ref), (void**)shdr_ref));
    return elfpv_stack_error(ELFPV_OK);
}

int elfpv_get_shdr_wname(const char* name, Elf64_Shdr** shdr_ref)
{
    Elf64_Ehdr* hdr;
    elfpv_check(elfpv_get_hdr(&hdr));

    Elf64_Shdr* shtext_sh;
    elfpv_check(elfpv_get_shdr_strtab_shdr(&shtext_sh));

    for (size_t i = 0; i < hdr->e_shnum; i++) {
        Elf64_Shdr* sh;
        elfpv_check(elfpv_get_shdr_windex(i, &sh));

        const char* sh_text;
        elfpv_check(elfpv_get_strtab_shdr_text(shtext_sh, sh->sh_name, &sh_text));

        if (strcmp(name, sh_text) == 0) {
            *shdr_ref = sh;
            return elfpv_stack_error(ELFPV_OK);
        }
    }

    return elfpv_stack_error(ELFPV_ERR_NOT_FOUND);
}

int elfpv_get_shdr_wtype(Elf64_Word type, Elf64_Shdr** shdr_ref)
{
    Elf64_Ehdr* hdr;
    elfpv_check(elfpv_get_hdr(&hdr));

    for (size_t i = 0; i < hdr->e_shnum; i++) {
        Elf64_Shdr* sh;
        elfpv_check(elfpv_get_shdr_windex(i, &sh));

        if (sh->sh_type == type) {
            *shdr_ref = sh;
            return elfpv_stack_error(ELFPV_OK);
        }
    }

    return elfpv_stack_error(ELFPV_ERR_NOT_FOUND);
}

int elfpv_print_shdr(Elf64_Shdr* shdr)
{
    Elf64_Ehdr* hdr;
    elfpv_check(elfpv_get_hdr(&hdr));

    Elf64_Shdr* shtext_sh;
    elfpv_check(elfpv_get_shdr_strtab_shdr(&shtext_sh));

    const char* shtext;
    elfpv_check(elfpv_get_strtab_shdr_text(shtext_sh, shdr->sh_name, &shtext));
    size_t shtext_len = strlen(shtext);

    elfpv_newl("%s", "");
    elfpv_snewl(shtext, shtext_len);
    elfpv_unewl(shdr->sh_name);
    elfpv_unewl(shdr->sh_type);
    elfpv_unewl(shdr->sh_flags);
    elfpv_unewl(shdr->sh_offset);
    elfpv_unewl(shdr->sh_size);
    elfpv_unewl(shdr->sh_link);
    elfpv_unewl(shdr->sh_info);
    elfpv_unewl(shdr->sh_addralign);
    elfpv_unewl(shdr->sh_entsize);

    return elfpv_stack_error(ELFPV_OK);
}

int elfpv_strtab_shdr_type(Elf64_Shdr* strtab_sh)
{
    int result = strtab_sh->sh_type == SHT_STRTAB;
    return elfpv_stack_error((result) ? ELFPV_OK : ELFPV_ERR_TYPE);
}

int elfpv_get_strtab_shdr_text(Elf64_Shdr* strtab_sh, size_t index, const char** text_ref)
{
    elfpv_check(elfpv_strtab_shdr_type(strtab_sh));
    size_t offset = strtab_sh->sh_offset + index;
    elfpv_check(elfpv_offset(offset, sizeof(**text_ref), (void**)text_ref));
    return elfpv_stack_error(ELFPV_OK);
}

int elfpv_print_strtab_shdr_text(Elf64_Shdr* strtab_sh)
{
    elfpv_check(elfpv_strtab_shdr_type(strtab_sh));
    elfpv_newl("%s", "");
    for (size_t i = 1; i < strtab_sh->sh_size; i++) {
        const char* str;
        elfpv_check(elfpv_get_strtab_shdr_text(strtab_sh, i, &str));
        size_t str_len = strlen(str);
        i += str_len;
        elfpv_snewl(str, str_len);
    }
    return elfpv_stack_error(ELFPV_OK);
}

int elfpv_sym_shdr_type(Elf64_Shdr* sym_sh)
{
    int result = sym_sh->sh_type == SHT_SYMTAB;
    return elfpv_stack_error((result) ? ELFPV_OK : ELFPV_ERR_TYPE);
}

int elfpv_get_sym_strtab_shdr(Elf64_Shdr* sym_sh, Elf64_Shdr** shdr_ref)
{
    elfpv_check(elfpv_sym_shdr_type(sym_sh));

    Elf64_Shdr* strtab_sh;
    elfpv_check(elfpv_get_shdr_windex(sym_sh->sh_link, &strtab_sh));
    elfpv_check(elfpv_strtab_shdr_type(strtab_sh));

    *shdr_ref = strtab_sh;
    return elfpv_stack_error(ELFPV_OK);
}

int elfpv_get_sym_num(Elf64_Shdr* sym_sh, size_t* num_ref)
{
    elfpv_check(elfpv_sym_shdr_type(sym_sh));

    if (sym_sh->sh_entsize == 0) {
        return elfpv_stack_error(ELFPV_ERR_DIV_ZERO);
    }

    size_t sym_num = sym_sh->sh_size / sym_sh->sh_entsize;
    *num_ref = sym_num;
    return elfpv_stack_error(ELFPV_OK);
}

int elfpv_get_sym_windex(Elf64_Shdr* sym_sh, size_t index, Elf64_Sym** sym_ref)
{
    elfpv_check(elfpv_sym_shdr_type(sym_sh));

    size_t sym_num;
    elfpv_check(elfpv_get_sym_num(sym_sh, &sym_num));

    if (index >= sym_num) {
        elfpv_stack_error(ELFPV_ERR_INDEX);
    }

    size_t offset = sym_sh->sh_offset + index * sym_sh->sh_entsize;
    Elf64_Sym* sym;
    elfpv_check(elfpv_offset(offset, sym_sh->sh_entsize, (void**)&sym));

    *sym_ref = sym;
    return elfpv_stack_error(ELFPV_OK);
}

int elfpv_get_sym_wname(Elf64_Shdr* sym_sh, const char* name, Elf64_Sym** sym_ref)
{
    elfpv_check(elfpv_sym_shdr_type(sym_sh));

    Elf64_Shdr* strtab_sh;
    elfpv_check(elfpv_get_sym_strtab_shdr(sym_sh, &strtab_sh));

    size_t sym_num;
    elfpv_check(elfpv_get_sym_num(sym_sh, &sym_num));

    for (size_t i = 0; i < sym_num; i++) {
        size_t offset = sym_sh->sh_offset + i * sym_sh->sh_entsize;
        Elf64_Sym* sym;
        elfpv_check(elfpv_offset(offset, sym_sh->sh_entsize, (void**)&sym));

        const char* sym_name;
        elfpv_check(elfpv_get_strtab_shdr_text(strtab_sh, sym->st_name, &sym_name));

        if (strcmp(sym_name, name) == 0) {
            *sym_ref = sym;
            return elfpv_stack_error(ELFPV_OK);
        }
    }

    return elfpv_stack_error(ELFPV_ERR_NOT_FOUND);
}

int elfpv_get_sym_wtype(Elf64_Shdr* sym_sh, uint16_t type, int64_t prev_index, int64_t* index)
{
    elfpv_check(elfpv_sym_shdr_type(sym_sh));

    Elf64_Shdr* strtab_sh;
    elfpv_check(elfpv_get_sym_strtab_shdr(sym_sh, &strtab_sh));

    size_t sym_num;
    elfpv_check(elfpv_get_sym_num(sym_sh, &sym_num));
    printf("%li %lu\n", prev_index, sym_num);

    if (prev_index >= 0) {
        size_t u_prev_index = (size_t)prev_index;
        if (u_prev_index > sym_num) {
            return elfpv_stack_error(ELFPV_ERR_INDEX);
        }
    }

    if (prev_index < 0) {
        prev_index = -1;
    }

    for (int64_t i = prev_index + 1; i < sym_num; i++) {
        size_t offset = sym_sh->sh_offset + i * sym_sh->sh_entsize;
        Elf64_Sym* sym;
        elfpv_check(elfpv_offset(offset, sym_sh->sh_entsize, (void**)&sym));

        uint16_t sym_type = ELF64_ST_TYPE(sym->st_info);
        if (sym_type == type) {
            *index = i;
            return elfpv_stack_error(ELFPV_OK);
        }
    }

    *index = -1;
    return elfpv_stack_error(ELFPV_OK);
}

int elfpv_print_sym(Elf64_Shdr* sym_sh, Elf64_Sym* sym)
{
    Elf64_Shdr* strtab_sh;
    elfpv_check(elfpv_get_sym_strtab_shdr(sym_sh, &strtab_sh));

    const char* sym_name;
    elfpv_check(elfpv_get_strtab_shdr_text(strtab_sh, sym->st_name, &sym_name));
    size_t name_length = strlen(sym_name);

    unsigned sym_type = ELF64_ST_TYPE(sym->st_info);
    unsigned sym_bind = ELF64_ST_BIND(sym->st_info);
    unsigned sym_visb = ELF64_ST_VISIBILITY(sym->st_other);

    elfpv_newl("%s", "");
    elfpv_snewl(sym_name, name_length);
    elfpv_unewl(sym->st_name);
    elfpv_unewl(sym->st_info);
    elfpv_unewl(sym_type);
    elfpv_unewl(sym_bind);
    elfpv_unewl(sym->st_other);
    elfpv_unewl(sym_visb);
    elfpv_unewl(sym->st_shndx);
    elfpv_unewl(sym->st_value);
    elfpv_unewl(sym->st_size);

    return elfpv_stack_error(ELFPV_OK);
}

int elfpv_print_sym_shdr(Elf64_Shdr* sym_sh)
{
    elfpv_check(elfpv_sym_shdr_type(sym_sh));

    size_t sym_num;
    elfpv_check(elfpv_get_sym_num(sym_sh, &sym_num));

    for (size_t i = 0; i < sym_num; i++) {
        Elf64_Sym* sym;
        elfpv_check(elfpv_get_sym_windex(sym_sh, i, &sym));
        elfpv_check(elfpv_print_sym(sym_sh, sym));
    }

    return elfpv_stack_error(ELFPV_OK);
}

void elfpv_impl_bnewl(const char* expr, uint8_t* buff, size_t size)
{
    size_t expr_len = strlen(expr);
    elfpv_newl("%-30s %-12zu %-12s", expr, size, "...");
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
        elfpv_gnewl("%-12hhx", name_buf, (size_t)1, buff[i]);
    }
}
