/*
    Copyright (c) 2025 Gaël Fortier <gael.fortier.1@ens.etsmtl.ca>
*/

#include "utils.h"
#include "lib.h"
#include <ctype.h>
#include <stdio.h>

int elf_tokenize(char* text, size_t max_tok, char** tokens, int* tok_num)
{
    if (text == NULL || tokens == NULL || tok_num == NULL) {
        return elf_stack_error(ELF_ERR_NULL);
    }

    char* src = text;
    char* token = NULL;

    *tok_num = 0;
    while ((token = strtok(src, " \n")) && *tok_num < max_tok) {
        src = NULL;
        tokens[*tok_num] = token;
        (*tok_num)++;
    }

    return elf_stack_error(ELF_OK);
}

int elf_execute(int argc, char** argv, elf_cmdent* list, size_t size)
{
    if (argc == 0) {
        elf_print_cmdent_list(list, size);
        return elf_stack_error(ELF_ERR_ARGC);
    }
    if (!argv) {
        return elf_stack_error(ELF_ERR_NULL);
    }
    if (!list) {
        return elf_stack_error(ELF_ERR_NULL);
    }

    for (size_t i = 0; i < size; i++) {
        if (strcmp(argv[0], list[i].name) == 0) {
            int result = list[i].command(argc - 1, argv + 1);
            return elf_stack_error(result);
        }
    }

    elf_print_cmdent_list(list, size);
    return elf_stack_error(ELF_ERR_NOT_FOUND);
}

int elf_text_isnum(const char* text, long* number)
{
    if (!text || !number) {
        return elf_stack_error(ELF_ERR_NULL);
    }

    long n = 0;
    long e = 1;

    while (isdigit(*text) && *text != '\0') {
        n += ((*text) - '0') * e;
        e *= 10;
        text++;
    }

    *number = (*text == '\0') ? n : 0;
    return elf_stack_error((*text == '\0') ? ELF_OK : ELF_ERR_NOT_NUMBER);
}

int elf_print_cmdent_list(elf_cmdent* list, size_t size)
{
    if (!list) {
        return elf_stack_error(ELF_ERR_NULL);
    }

    for (size_t i = 0; i < size; i++) {
        printf("%s\n", list[i].name);
    }

    return elf_stack_error(ELF_OK);
}

int elf_hexstr_tobyte(const char* text, uint8_t* byte_ref)
{
    if (!text) {
        return elf_stack_error(ELF_ERR_NULL);
    }

    if (!byte_ref) {
        return elf_stack_error(ELF_ERR_NULL);
    }

    uint64_t text_len = strlen(text);
    if (text_len != 2) {
        return elf_stack_error(ELF_ERR_NOT_NUMBER);
    }

    int result = sscanf(text, "%hhx", byte_ref);
    return elf_stack_error((result == 0) ? ELF_ERR_NOT_NUMBER : ELF_OK);
}
