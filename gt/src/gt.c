/*
    Copyright (c) 2026 Gaël Fortier <gael.fortier.1@ens.etsmtl.ca>
*/

#include "elf/lib.h"
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

        exit(result);
    }
}

int strswitch_impl(const char* str, ...)
{
    va_list argv;
    va_start(argv, str);

    size_t i = 0;
    const char* it;

    do {
        it = va_arg(argv, const char*);
        i++;
    } while (strcmp(it, str) != 0 && it != NULL);

    if (it == NULL) {
        i = -1;
    }

    va_end(argv);
    return i;
}

#define strswitch(str, ...) \
    switch (strswitch_impl(str, __VA_ARGS__, NULL))

int main(int argc, char** argv)
{
    const char* execpath = argv[0];
    check(argc < 2);

    size_t sym_num;
    int64_t prev_index;
    int64_t current_index;
    elf_index_iterator it;
    Elf64_Ehdr* header;
    Elf64_Shdr* symtabsh;
    Elf64_Shdr* strsh;
    Elf64_Sym* sym;
    const char* name;

    int result = 0;
    strswitch(argv[1], "elf")
    {
    case 0: // not found
        check(1);
        break;
    case 1: // elf
        check(argc < 4);
        check(elf_load_elf(argv[2]));

        strswitch(argv[3], "header", "funclist", "func")
        {
        case 0: // not found
            check(1);
            break;
        case 1: // header
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
            break;
        case 2: // funclist
            check(elf_get_sym_shdr(&symtabsh));
            check(elf_get_sym_strtab_shdr(symtabsh, &strsh));
            check(elf_reset_index_iterator(&it));
            do {
                check(elf_get_sym_wtype(symtabsh, STT_FUNC, &it));
                sym = it.object;
                uint8_t sym_visibility = ELF64_ST_VISIBILITY(sym->st_other);
                if (sym_visibility != STV_HIDDEN) {
                    check(elf_get_strtab_shdr_text(strsh, sym->st_name, &name));
                    printf("%s\n", name);
                }
            } while (!it.done);
            break;
        case 3: // func
            check(argc < 5);
            name = argv[4];
            check(elf_get_sym_shdr(&symtabsh));
            check(elf_get_sym_strtab_shdr(symtabsh, &strsh));
            result = elf_get_sym_wname(symtabsh, name, &sym);
            if (result == ELF_OK) {
                check(elf_get_strtab_shdr_text(strsh, sym->st_name, &name));
                printf("%s found\n", name);
                check(elf_print_sym(symtabsh, sym));
            } else {
                printf("%s not found\n", name);
            }
            break;
        }
        break;
    }
    elf_unload_elf();
    return 0;
}

/*
 * while (index != -1) {
        elfpv_check(elfpv_get_sym_wtype(sym_sh, type, prev, &index));
        if (index != -1) {
            count++;
        }
        prev = index;
    }
 *
 *
 *
e_type      2
e_machine   62
e_entry     4200896
e_phoff     64
e_shoff     846840
e_flags     0
e_ehsize    64
e_phentsize 56
e_phnum     10
e_shentsize 64
e_shnum     34
e_shstrndx  33

./gt.elf elf "out/elf/lib.o" header | grep -Eo "e_entry +[0-9]+"
 */
