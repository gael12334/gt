/*
    Copyright (c) 2025 Gaël Fortier <gael.fortier.1@ens.etsmtl.ca>
*/

#pragma once
#include <ctype.h>
#include <stddef.h>

typedef int (*elfpv_command)(int, char**);

typedef struct {
    char* name;
    elfpv_command command;
} elfpv_cmdent;

int elfpv_tokenize(char* text, size_t max_tok, char** tokens, int* tok_num);

int elfpv_execute(int argc, char** argv, elfpv_cmdent* list, size_t size);

int elfpv_print_cmdent_list(elfpv_cmdent* list, size_t size);

int elfpv_text_isnum(const char* text, long* number);
