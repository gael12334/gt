/*
    Copyright (c) 2026 Gaël Fortier <gael.fortier.1@ens.etsmtl.ca>
*/

#include "lib.h"
#include "patch.h"
#include <elf.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void check(int result)
{
    if (result) {
        if (elf_elf_loaded()) {
            elf_unload_elf();
        }
        elf_print_error_trace();
        printf("%i\n", result);
        exit(-1);
    }
}

int cmd_elf_header(const char* filename)
{
    check(elf_load_elf(filename));
    Elf64_Ehdr* header;
    check(elf_get_hdr(&header));
    printf("%s %hu\n", "e_type     ", header->e_type);
    printf("%s %hu\n", "e_machine  ", header->e_machine);
    printf("%s %lu\n", "e_entry    ", header->e_entry);
    printf("%s %lu\n", "e_phoff    ", header->e_phoff);
    printf("%s %lu\n", "e_shoff    ", header->e_shoff);
    printf("%s %hu\n", "e_flags    ", header->e_flags);
    printf("%s %hu\n", "e_ehsize   ", header->e_ehsize);
    printf("%s %hu\n", "e_phentsize", header->e_phentsize);
    printf("%s %hu\n", "e_phnum    ", header->e_phnum);
    printf("%s %hu\n", "e_shentsize", header->e_shentsize);
    printf("%s %hu\n", "e_shnum    ", header->e_shnum);
    printf("%s %hu\n", "e_shstrndx ", header->e_shstrndx);
    check(elf_unload_elf());
    return 0;
}

int cmd_elf_funclist(const char* filename)
{
    check(elf_load_elf(filename));
    elf_index_iterator it;
    Elf64_Shdr* symtabsh;
    Elf64_Shdr* strsh;
    const char* name;
    Elf64_Sym* sym;
    int error;

    check(elf_get_sym_shdr(&symtabsh));
    check(elf_get_sym_strtab_shdr(symtabsh, &strsh));
    check(elf_reset_index_iterator(&it));
    while ((error = elf_get_sym_wtype(symtabsh, STT_FUNC, &it) == ELF_OK)) {
        sym = it.object;
        uint8_t sym_visibility = ELF64_ST_VISIBILITY(sym->st_other);
        if (sym_visibility != STV_HIDDEN) {
            check(elf_get_strtab_shdr_text(strsh, sym->st_name, &name));
            printf("%s\n", name);
        }
    }
    check(error);
    check(elf_unload_elf());
    return 0;
}

int cmd_elf_func(int argc, char** argv, const char* filename)
{
    if (argc >= 5) {
        const char* name = argv[4];
        check(elf_load_elf(filename));
        Elf64_Shdr* symtabsh;
        check(elf_get_sym_shdr(&symtabsh));
        Elf64_Shdr* strsh;
        check(elf_get_sym_strtab_shdr(symtabsh, &strsh));
        Elf64_Sym* sym;
        check(elf_get_sym_wname(symtabsh, name, &sym));
        check(elf_get_strtab_shdr_text(strsh, sym->st_name, &name));
        check(elf_print_sym(symtabsh, sym));
        check(elf_unload_elf());
        return 0;
    }

    return 1;
}

int cmd_elf_patch(const char* filename)
{
    check(elf_patch_elf(filename));
    return 0;
}

int cmd_elf(int argc, char** argv)
{
    if (argc >= 3) {
        const char* filename = argv[2];
        if (argc >= 4 && strcmp(argv[3], "header") == 0) {
            return cmd_elf_header(filename);
        } else if (argc >= 4 && strcmp(argv[3], "funclist") == 0) {
            return cmd_elf_funclist(filename);
        } else if (argc >= 4 && strcmp(argv[3], "func") == 0) {
            return cmd_elf_func(argc, argv, filename);
        } else if (argc >= 4 && strcmp(argv[3], "patch") == 0) {
            return cmd_elf_patch(filename);
        }
    }
    return 1;
}

int main(int argc, char** argv)
{
    if (argc >= 2) {
        int result = 0;
        if (strcmp(argv[1], "elf") == 0) {
            result = cmd_elf(argc, argv);
        }

        printf("%i\n", result);
        if (result) {
            elf_print_error_trace();
        }

    } else {
        puts("expected at least 2 arguments.");
    }
    return 1;
}
