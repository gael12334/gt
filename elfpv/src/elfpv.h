/*
    Copyright © 2025 Gaël Fortier <gael.fortier.1@ens.etsmtl.ca>
*/

#pragma once
#include "elfpv_buffer.h"
#include <elf.h>
#include <stddef.h>

typedef struct {
    long error;
    const char* message;
    const char* name;
    const char* text;
    Elf64_Sym symbol;
} elf_function_text;

elfpv_buffer elf_load(const char* path, size_t* not_null_size);
void elf_menu(const char* filename);
void elf_cli_menu(const char* filename, char* cli);
void elf_free(char* buffer);
void elf_set_symbol_name(const char* hijack, const char* hook);
