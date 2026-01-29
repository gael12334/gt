/*
    Copyright (c) 2026 Gaël Fortier <gael.fortier.1@ens.etsmtl.ca>
*/

#include "cli.h"
#include "elf.h"
#include "patch.h"
#include <elf.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void check(int result)
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
    elf_check(elf_load_elf(filename));
    elf_check(elf_print_header());
    elf_check(elf_unload_elf());
    return elf_stack_error(ELF_OK);
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

    elf_check(elf_get_sym_shdr(&symtabsh));
    elf_check(elf_get_sym_strtab_shdr(symtabsh, &strsh));
    elf_check(elf_reset_index_iterator(&it));
    while ((error = elf_get_sym_wtype(symtabsh, STT_FUNC, &it) == ELF_OK)) {
        sym = it.object;
        uint8_t sym_visibility = ELF64_ST_VISIBILITY(sym->st_other);
        if (sym_visibility != STV_HIDDEN) {
            elf_check(elf_get_strtab_shdr_text(strsh, sym->st_name, &name));
            printf("%s\n", name);
        }
    }
    elf_check(error);
    elf_check(elf_unload_elf());
    return elf_stack_error(ELF_OK);
}

int cmd_elf_func(int argc, char** argv, const char* filename)
{
    if (argc >= 5) {
        const char* name = argv[4];
        Elf64_Shdr* symtabsh;
        Elf64_Shdr* strsh;
        Elf64_Sym* sym;
        elf_check(elf_load_elf(filename));
        elf_check(elf_get_sym_shdr(&symtabsh));
        elf_check(elf_get_sym_strtab_shdr(symtabsh, &strsh));
        elf_check(elf_get_sym_wname(symtabsh, name, &sym));
        elf_check(elf_get_strtab_shdr_text(strsh, sym->st_name, &name));
        elf_check(elf_print_sym(symtabsh, sym));
        elf_check(elf_unload_elf());
        return elf_stack_error(ELF_OK);
    } else {
        printf("elf: func: <function name>");
        return elf_stack_error(ELF_OK);
    }
}

int cmd_elf_patch(const char* filename, const char* entry, const char* verbose)
{
    int verbose_enabled = (verbose != NULL && strcmp(verbose, "verbose") == 0);

    elf_check(elf_load_elf(filename));
    elf_check(elf_patch_elf(entry, verbose_enabled));
    elf_check(elf_save_elf("out"));
    elf_check(elf_unload_elf());
    return elf_stack_error(ELF_OK);
}

int cmd_elf_program(const char* filename)
{
    int error = 0;
    Elf64_Ehdr* header;
    Elf64_Phdr* prog_header;

    elf_check(elf_load_elf(filename));
    elf_check(elf_get_hdr(&header));

    for (size_t i = 0; i < header->e_phnum; i++) {
        puts("");
        elf_check(elf_get_phdr_windex(i, &prog_header));
        elf_check(elf_print_phdr(prog_header));
    }
    elf_check(elf_unload_elf());
    return elf_stack_error(ELF_OK);
}

int cmd_elf_section(const char* filename)
{
    int error = 0;
    Elf64_Ehdr* header;
    Elf64_Shdr* section_header;

    elf_check(elf_load_elf(filename));
    elf_check(elf_get_hdr(&header));

    for (size_t i = 0; i < header->e_shnum; i++) {
        puts("");
        elf_check(elf_get_shdr_windex(i, &section_header));
        elf_check(elf_print_shdr(section_header));
    }
    elf_check(elf_unload_elf());
    return elf_stack_error(ELF_OK);
}

int cmd_elf(int argc, char** argv)
{
    if (argc >= 3) {
        const char* filename = argv[2];
        if (argc >= 4 && strcmp(argv[3], "header") == 0) {
            elf_check(cmd_elf_header(filename));
            return elf_stack_error(ELF_OK);
        } else if (argc >= 4 && strcmp(argv[3], "funclist") == 0) {
            elf_check(cmd_elf_funclist(filename));
            return elf_stack_error(ELF_OK);
        } else if (argc >= 4 && strcmp(argv[3], "func") == 0) {
            elf_check(cmd_elf_func(argc, argv, filename));
            return elf_stack_error(ELF_OK);
        } else if (argc >= 5 && strcmp(argv[3], "patch") == 0) {
            elf_check(cmd_elf_patch(filename, argv[4], NULL));
            return elf_stack_error(ELF_OK);
        } else if (argc >= 6 && strcmp(argv[3], "patch") == 0) {
            elf_check(cmd_elf_patch(filename, argv[4], argv[5]));
            return elf_stack_error(ELF_OK);
        } else if (argc >= 4 && strcmp(argv[3], "program") == 0) {
            elf_check(cmd_elf_program(filename));
            return elf_stack_error(ELF_OK);
        } else if (argc >= 4 && strcmp(argv[3], "section") == 0) {
            elf_check(cmd_elf_section(filename));
            return elf_stack_error(ELF_OK);
        } else {
            puts("elf: header, funclist, func, patch");
            return elf_stack_error(ELF_OK);
        }
    } else {
        puts("elf: <path to elf file>");
        return elf_stack_error(ELF_OK);
    }
}

int main(int argc, char** argv)
{
    if (argc < 2) {
        puts("expected at least 2 arguments.");
        return 1;
    } else if (strcmp(argv[1], "elf") == 0) {
        check(cmd_elf(argc, argv));
        return 0;
    } else {
        puts("...: elf");
        return 1;
    }
}
