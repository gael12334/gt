/*
    Copyright (c) 2025 Gaël Fortier <gael.fortier.1@ens.etsmtl.ca>
*/

#include "elfpv_utils.h"
#include "elfpv_lib.h"
#include <ctype.h>
#include <stdio.h>

int elfpv_tokenize(char* text, size_t max_tok, char** tokens, int* tok_num)
{
    if (text == NULL || tokens == NULL || tok_num == NULL) {
        return elfpv_stack_error(ELFPV_ERR_NULL);
    }

    char* src = text;
    char* token = NULL;

    *tok_num = 0;
    while ((token = strtok(src, " \n")) && *tok_num < max_tok) {
        src = NULL;
        tokens[*tok_num] = token;
        (*tok_num)++;
    }

    return elfpv_stack_error(ELFPV_OK);
}

int elfpv_execute(int argc, char** argv, elfpv_cmdent* list, size_t size)
{
    if (argc == 0) {
        elfpv_print_cmdent_list(list, size);
        return elfpv_stack_error(ELFPV_ERR_ARGC);
    }
    if (!argv) {
        return elfpv_stack_error(ELFPV_ERR_NULL);
    }
    if (!list) {
        return elfpv_stack_error(ELFPV_ERR_NULL);
    }

    for (size_t i = 0; i < size; i++) {
        if (strcmp(argv[0], list[i].name) == 0) {
            int result = list[i].command(argc - 1, argv + 1);
            return elfpv_stack_error(result);
        }
    }

    elfpv_print_cmdent_list(list, size);
    return elfpv_stack_error(ELFPV_ERR_NOT_FOUND);
}

int elfpv_text_isnum(const char* text, long* number)
{
    if (!text || !number) {
        return elfpv_stack_error(ELFPV_ERR_NULL);
    }

    long n = 0;
    long e = 1;

    while (isdigit(*text) && *text != '\0') {
        n += ((*text) - '0') * e;
        e *= 10;
        text++;
    }

    *number = (*text == '\0') ? n : 0;
    return elfpv_stack_error((*text == '\0') ? ELFPV_OK : ELFPV_ERR_NOT_NUMBER);
}

int elfpv_print_cmdent_list(elfpv_cmdent* list, size_t size)
{
    if (!list) {
        return elfpv_stack_error(ELFPV_ERR_NULL);
    }

    for (size_t i = 0; i < size; i++) {
        printf("%s\n", list[i].name);
    }

    return elfpv_stack_error(ELFPV_OK);
}

int elfpv_hexstr_tobyte(const char* text, uint8_t* byte_ref)
{
    if (!text) {
        return elfpv_stack_error(ELFPV_ERR_NULL);
    }

    if (!byte_ref) {
        return elfpv_stack_error(ELFPV_ERR_NULL);
    }

    uint64_t text_len = strlen(text);
    if (text_len != 2) {
        return elfpv_stack_error(ELFPV_ERR_NOT_NUMBER);
    }

    int result = sscanf(text, "%hhx", byte_ref);
    return elfpv_stack_error((result == 0) ? ELFPV_ERR_NOT_NUMBER : ELFPV_OK);
}
