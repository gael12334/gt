/*
    Copyright (c) 2026 Gaël Fortier <gael.fortier.1@ens.etsmtl.ca>
*/

#pragma once

int cmd_elf_header(const char* filename);
int cmd_elf_funclist(const char* filename);
int cmd_elf_func(int argc, char** argv, const char* filename);
int cmd_elf_patch(const char* filename, const char* entry, const char* verbose);
int cmd_elf_program(const char* filename);
int cmd_elf_section(const char* filename);
int cmd_elf(int argc, char** argv);
