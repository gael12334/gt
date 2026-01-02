/*
    Copyright (c) 2025 Gaël Fortier <gael.fortier.1@ens.etsmtl.ca>
*/

#pragma once
#include <ctype.h>
#include <stddef.h>
#include <stdint.h>

typedef int (*elf_command)(int, char**);

typedef struct {
    char* name;
    elf_command command;
} elf_cmdent;

int elf_tokenize(char* text, size_t max_tok, char** tokens, int* tok_num);

int elf_execute(int argc, char** argv, elf_cmdent* list, size_t size);

int elf_print_cmdent_list(elf_cmdent* list, size_t size);

int elf_text_isnum(const char* text, long* number);

int elf_hexstr_tobyte(const char* text, uint8_t* byte_ref);
