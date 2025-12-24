/*
    Copyright © 2025 Gaël Fortier <gael.fortier.1@ens.etsmtl.ca>
*/

#include "patcher.h"
#include <ctype.h>
#include <elf.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#define sym(x) #x
#define print_bar(x) for(int i=0;i<x;i++) fputc((i<(x-1))?'-':'\n', stdout)
#define print_col(x)   printf("%-30s : %-5s %-8s %s\n", x, "size", "data", "hex")
#define print_enm(x,e) printf("%-30s : %-5zu %-8u (%X) <%s>\n", #x, sizeof(x), x, x, e[x])
#define print_ven(x,e) printf("%-30s : %-5zu %-8u (%X) <%s>\n", #x, sizeof(x), x, x, e)
#define print_u08(x)   printf("%-30s : %-5zu %-8hhu (%hhX)\n", #x, sizeof(x), x, x)
#define print_u16(x)   printf("%-30s : %-5zu %-8hu (%hX)\n", #x, sizeof(x), x, x)
#define print_u32(x)   printf("%-30s : %-5zu %-8u (%X)\n", #x, sizeof(x), x, x)
#define print_u64(x)   printf("%-30s : %-5zu %-8lu (%lX)\n", #x, sizeof(x), x, x)
#define print_str(x)   printf("%-30s : %-5zu %s\n", #x, strlen(x), x)
#define print_buf(x,n) printf("%-30s : %-5zu ", #x, (size_t)n); for(int _xijk = 0; _xijk < n; _xijk++) printf("%02hhX ", x[_xijk]); fputc('\n', stdout)
#define bar_length 55
#define TEXT_NOP (0x90)
#define TEXT_ENDBR64 (0xFA1E0FF3)
#define TEXT_PADDING (50)

static const char* section_type[SHT_NUM] = {
    "SHT_NULL\0         ",
    "SHT_PROGBITS\0     ",
    "SHT_SYMTAB\0       ",
    "SHT_STRTAB\0       ",
    "SHT_RELA\0         ",
    "SHT_HASH\0         ",
    "SHT_DYNAMIC\0      ",
    "SHT_NOTE\0         ",
    "SHT_NOBITS\0       ",
    "SHT_REL\0          ",
    "SHT_SHLIB\0        ",
    "SHT_DYNSYM\0       ",
    "SHT_INIT_ARRAY\0   ",
    "SHT_FINI_ARRAY\0   ",
    "SHT_PREINIT_ARRAY\0",
    "SHT_GROUP\0        ",
    "SHT_SYMTAB_SHNDX\0 ",
    "SHT_RELR\0         ",
    "SHT_NUM\0          ",
    "???\0              "
};

static const char* symbol_bindings[STB_NUM] = {
    "STB_LOCAL\0   ",
    "STB_GLOBAL\0  ",
    "STB_WEAK\0    ",
};

static const char* symbol_types[STT_NUM] = {
    "STT_NOTYPE\0   ",
    "STT_OBJECT\0   ",
    "STT_FUNC\0     ",
    "STT_SECTION\0  ",
    "STT_FILE\0     ",
    "STT_COMMON\0   ",
    "STT_TLS\0      ",
};

// ---- header ----

Elf64_Ehdr* elf_get_header(char* buffer) {
    return (void*)buffer;
}

void elf_print_header(Elf64_Ehdr* header) {
    print_col("Elf64_Ehdr");
    print_bar(bar_length);
    print_buf(header->e_ident, sizeof(header->e_ident));
    print_u16(header->e_type);
    print_u16(header->e_machine);
    print_u32(header->e_version);
    print_u64(header->e_entry);
    print_u64(header->e_phoff);
    print_u64(header->e_shoff);
    print_u32(header->e_flags);
    print_u16(header->e_ehsize);
    print_u16(header->e_phentsize);
    print_u16(header->e_phnum);
    print_u16(header->e_shentsize);
    print_u16(header->e_shnum);
    print_u16(header->e_shstrndx);
    print_bar(bar_length);
}

// ---- section ----

Elf64_Shdr* elf_get_section_list(Elf64_Ehdr* header) {
    return (void*)header + header->e_shoff;
}

Elf64_Shdr* elf_get_section_by_index(Elf64_Ehdr* header, size_t index) {
    Elf64_Shdr* section_list = elf_get_section_list(header);
    return (index < header->e_shnum) ? section_list + index : NULL;
}

char* elf_get_section_name_list(Elf64_Ehdr* header, size_t* opt_size) {
    Elf64_Shdr* sections = elf_get_section_list(header);
    Elf64_Shdr* name_section = sections + header->e_shstrndx;
    char* names =  (void*) header + name_section->sh_offset;
    if(opt_size) { *opt_size = name_section->sh_size; }
    return names;
}

Elf64_Shdr* elf_get_section_by_name(Elf64_Ehdr* header, const char* name) {
    Elf64_Shdr* section_list = elf_get_section_list(header);
    char* section_names = elf_get_section_name_list(header, NULL);
    for(size_t i = 0; i < header->e_shnum; i++) {
        Elf64_Shdr* selected = section_list + i;
        int selected_matches = strcmp(section_names + selected->sh_name, name) == 0;
        if(selected_matches) { return selected; }
    }
    return NULL;
}

void elf_print_section(Elf64_Ehdr* header, Elf64_Shdr* section) {
    static char section_title[50];
    Elf64_Shdr* section_list = elf_get_section_list(header);
    const char* name_list = elf_get_section_name_list(header, NULL);
    size_t index = section - section_list;
    size_t offset = (void*)section - (void*)header;

    snprintf(section_title, sizeof(section_title), "sections[%zu] (offset %zX)", index, offset);
    const char* type = section_type[(section->sh_type < SHT_NUM) ? section->sh_type : SHT_NUM];

    print_col(section_title);
    print_bar(bar_length);
    print_enm(section->sh_name, &name_list);
    print_ven(section->sh_type, type);
    print_u64(section->sh_flags);
    print_u64(section->sh_addr);
    print_u64(section->sh_offset);
    print_u64(section->sh_size);
    print_u32(section->sh_link);
    print_u32(section->sh_info);
    print_u64(section->sh_addralign);
    print_u64(section->sh_entsize);
    print_bar(bar_length);
}

// ---- symbol ----

Elf64_Sym* elf_get_symbol_list(Elf64_Ehdr* header, size_t* opt_length) {
    Elf64_Shdr* symbol_section = elf_get_section_by_name(header, ".symtab");
    if(symbol_section == NULL) { return NULL; }
    if(opt_length) { opt_length[0] = symbol_section->sh_size / symbol_section->sh_entsize; }
    return (void*)header + symbol_section->sh_offset;
}

Elf64_Sym* elf_get_symbol_by_index(Elf64_Ehdr* header, size_t index) {
    size_t symbol_number = 0;
    Elf64_Sym* symbol_list = elf_get_symbol_list(header, &symbol_number);
    return (index < symbol_number) ? symbol_list + index : NULL;
}

char* elf_get_symbol_name_list(Elf64_Ehdr* header, size_t* opt_length) {
    Elf64_Shdr* name_section = elf_get_section_by_name(header, ".strtab");
    if(name_section == NULL) { return NULL; }
    if(opt_length) { *opt_length = name_section->sh_size / name_section->sh_entsize; }
    return (void*)header + name_section->sh_offset;
}

Elf64_Sym* elf_get_symbol_by_name(Elf64_Ehdr* header, const char* name) {
    size_t symbol_number = 0;
    Elf64_Sym* symbol_list = elf_get_symbol_list(header, &symbol_number);
    char* name_list = elf_get_symbol_name_list(header, NULL);
    for(size_t i = 0; i < symbol_number; i++) {
        Elf64_Sym* selected = symbol_list + i;
        int selected_matches = strcmp(name_list + selected->st_name, name);
        if(selected_matches == 0) { return selected; }
    }
    return NULL;
}

Elf64_Sym* elf_get_symbol_by_type(Elf64_Sym* symbol_list, size_t number, size_t type) {
    for(size_t i = 0; i < number; i++) {
        size_t symbol_type = ELF64_ST_TYPE(symbol_list[i].st_info);
        if(symbol_type == type) { return symbol_list + i; }
    }

    return NULL;
}

size_t elf_get_symbol_type(Elf64_Ehdr* header, Elf64_Sym* symbol) {
    size_t type = ELF64_ST_TYPE(symbol->st_info);
    return type;
}

const char* elf_get_symbol_type_name(Elf64_Ehdr* header, Elf64_Sym* symbol) {
    size_t type = elf_get_symbol_type(header, symbol);
    if(type >= STT_NUM) { return "invalid"; }
    const char* name = symbol_types[type];
    return name;
}

size_t elf_get_symbol_binding(Elf64_Ehdr* header, Elf64_Sym* symbol) {
    size_t binding = ELF64_ST_BIND(symbol->st_info);
    return binding;
}

const char* elf_get_symbol_binding_name(Elf64_Ehdr* header, Elf64_Sym* symbol) {
    size_t binding = elf_get_symbol_binding(header, symbol);
    if(binding >= STB_NUM) { return "invalid"; }
    const char* text = symbol_bindings[binding];
    return text;
}

void elf_print_symbol(Elf64_Ehdr* header, Elf64_Sym* symbol) {
    static char symbol_title[50];
    static char symbol_binding_type[30];
    Elf64_Sym* symbol_list = elf_get_symbol_list(header, NULL);
    char* name_list = elf_get_symbol_name_list(header, NULL);
    size_t symbol_index = symbol - symbol_list;
    size_t symbol_offset = (void*) symbol - (void*) header;

    const char* binding = elf_get_symbol_binding_name(header, symbol);
    const char* type = elf_get_symbol_type_name(header, symbol);
    snprintf(symbol_title, sizeof(symbol_title), "symbol [%zu] (offset %zX)", symbol_index, symbol_offset);
    snprintf(symbol_binding_type, sizeof(symbol_binding_type), "<%s> <%s>", binding, type);

    print_col(symbol_title);
    print_bar(bar_length);
    print_enm(symbol->st_name, &name_list);
    print_ven(symbol->st_info, symbol_binding_type);
    print_u08(symbol->st_other);
    print_u16(symbol->st_shndx);
    print_u64(symbol->st_value);
    print_u64(symbol->st_size);
    print_bar(bar_length);
}

// ---- patch ----

uint8_t* elf_get_function_text(Elf64_Ehdr* header, Elf64_Sym* symbol, size_t* not_null_size) {
    size_t type = elf_get_symbol_type(header, symbol);
    if(type >= STT_NUM) { return NULL; }

    *not_null_size = symbol->st_size;
    return (void*) header + symbol->st_value;
}

int elf_does_function_has_padding(uint8_t* text, size_t size, size_t padding_size) {
    if(size <= padding_size) { return 0; }
    size_t i = 0;
    uint32_t* endbr64 = (uint32_t*) text;
    int first_bytes_endbr64 = (*endbr64 == TEXT_ENDBR64);
    if(first_bytes_endbr64) { i += sizeof(*endbr64); }
    while(i < size && text[i++] == TEXT_NOP);
    return i - (first_bytes_endbr64 ? sizeof(*endbr64) : 0) >= padding_size;
}

void elf_patch_function_text(uint8_t* text, Elf64_Sym* hijack, Elf64_Sym* hook) {
    uint8_t patch[] = {
        0x90,                                       // nop
        0x48, 0x8b, 0x05, 0xFF, 0xFF, 0xFF, 0xFF,   // mov rax, qword ptr [hijack]    ; offset from next instruction to hijack symbol
        0x4c, 0x8b, 0x25, 0xed, 0xff, 0xff, 0xff,   // mov r12, qword ptr 0xedffffff  ; offset from next instruction to endbr64
        0x49, 0x3b, 0xc4,                           // cmp rax, r12
        0x0f, 0x84, 0x1a, 0x00, 0x00, 0x00,         // jz 0x1a                        ; jumps to start of function, after padding
        0x55,                                       // push rbp
        0x48, 0x89, 0xe5,                           // mov rbp, rsp
        0x48, 0x8b, 0x1d, 0xFF, 0xFF, 0xFF, 0xFF,   // mov rbx, qword ptr [hook]      ; offset from next instruction to hook symbol
        0x48, 0xc7, 0xc0, 0x00, 0x00, 0x00, 0x00,   // mov rax, 0x0
        0xff, 0xd3,                                 // call rbx
        0x5d,                                       // pop rbp
        0xc3                                        // ret
    };

    uint8_t* instructions[] = {
        patch,
        instructions[0] + 1,
        instructions[1] + 7,
        instructions[2] + 7,
        instructions[3] + 3,
        instructions[4] + 6,
        instructions[5] + 1,
        instructions[6] + 3,
        instructions[7] + 7,
        instructions[8] + 7,
        instructions[9] + 2,
        instructions[10] + 1,
    };

    size_t hijack_instruction_offset = instructions[2] - text;
    uint32_t hijack_offset = hijack->st_value - hijack_instruction_offset;
    memcpy(instructions[1] + 3, &hijack_offset, sizeof(hijack_offset));

    size_t hook_instruction_offset = instructions[8] - text;
    uint32_t hook_offset = hook->st_value - hook_instruction_offset;
    memcpy(instructions[7] + 3, &hook_offset, sizeof(hook_offset));
}

void elf_patch_all_functions(Elf64_Ehdr* header) {
    size_t symbol_number = 0;
    Elf64_Sym* symbol_list = elf_get_symbol_list(header, &symbol_number);
    Elf64_Sym* hijack_symbol = elf_get_symbol_by_name(header, "hijack");
    Elf64_Sym* hook_symbol = elf_get_symbol_by_name(header, "hook");

    if(symbol_list == NULL) {
        printf("Unable to patch : symbol list is empty.\n");
        return;
    }

    if(hijack_symbol == NULL || hook_symbol == NULL) {
        printf("Unable to patch : hijack and/or hook symbols are absent.\n");
        return;
    }

    for(size_t i = 0; i < symbol_number; i++) {
        size_t type = elf_get_symbol_type(header, symbol_list + i);
        if(type != STT_FUNC) { continue; }

        size_t text_size = 0;
        uint8_t* function_text = elf_get_function_text(header, symbol_list + i, &text_size);
        int valid_padding = elf_does_function_has_padding(function_text, text_size, TEXT_PADDING);
        if(!valid_padding) { continue; }

        elf_patch_function_text(function_text, hijack_symbol, hook_symbol);
    }
}

// ---- loading ----

char* elf_load(const char* path, size_t* not_null_size) {
    FILE* f = fopen(path, "r");
    if(f == NULL) { return NULL; }
    Elf64_Ehdr h;
    fread(&h, sizeof(h), 1, f);
    size_t fsz = h.e_shoff + h.e_shentsize * h.e_shnum;
    *not_null_size = h.e_ehsize + fsz;
    char* b = malloc(*not_null_size);
    memcpy(b, &h, sizeof(h));
    fread(b + sizeof(h), fsz - h.e_ehsize, 1, f);
    fclose(f);
    return b;
}

void elf_save(const char* path, char* buffer, size_t size) {
    FILE* file = fopen(path, "w");
    if(file == NULL) {
        printf("file error\n");
        return;
    }

    fwrite(buffer, size, 1, file);
    fclose(file);
}

void elf_free(char* buffer) {
    free(buffer);
}

// ---- menu ----

void elf_menu_print_symbol(Elf64_Ehdr* header, char* arg2) {
    for(char* it = arg2; *it != '\0'; it++) {
        if(!isdigit(*it)) {
            Elf64_Sym* symbol = elf_get_symbol_by_name(header, arg2);
            (symbol == NULL) ? printf("Invalid symbol name.\n") : elf_print_symbol(header, symbol);
            return;
        }
    }

    size_t index = atol(arg2);
    Elf64_Sym* symbol = elf_get_symbol_by_index(header, index);
    (symbol == NULL) ? printf("Invalid symbol index.\n") : elf_print_symbol(header, symbol);
}

void elf_menu_print_section(Elf64_Ehdr* header, char* arg2) {
    for(char* it = arg2; *it != '\0'; it++) {
        if(!isdigit(*it)) {
            Elf64_Shdr* section = elf_get_section_by_name(header, arg2);
            (section == NULL) ? printf("Invalid section name.\n") : elf_print_section(header, section);
            return;
        }
    }

    size_t index = atol(arg2);
    Elf64_Shdr* section = elf_get_section_by_index(header, index);
    (section == NULL) ? printf("Invalid section index.\n") : elf_print_section(header, section);
}

void elf_menu_print_all_sections(Elf64_Ehdr* header) {
    Elf64_Shdr* section_list = elf_get_section_list(header);
    for(size_t i = 0; i <  header->e_shnum; i++) {
        elf_print_section(header, section_list + i);
    }
}

void elf_menu_print_all_symbols(Elf64_Ehdr* header) {
    size_t symbol_number = 0;
    Elf64_Sym* symbol_list = elf_get_symbol_list(header, &symbol_number);

    printf("%zu\n", symbol_number);
    for(size_t i = 0; i < symbol_number; i++) {
        elf_print_symbol(header, symbol_list + i);
    }
}

void elf_menu_print(Elf64_Ehdr* header) {
    elf_print_header(header);
    elf_menu_print_all_sections(header);
    elf_menu_print_all_symbols(header);
}

void elf_menu_help(void) {
    const char* commands[] = {
        "help           : shows this.",
        "show           : shows everything.",
        "  header       : shows the elf header.",
        "  section      : shows all the sections.",
        "    <name>     : shows section with specified name.",
        "    <index>    : shows section at specified index.",
        "  symbol       : shows all the symbols.",
        "    <name>     : shows symbol with specified name.",
        "    <index>    : shows symbol at specified index",
        "patch          : <not implemeneted>\n                   fills function padding with machine\n                   code to hijack function calls.",
        "quit           : returns to terminal.",
        NULL
    };

    printf("help menu\n");
    print_bar(55);
    for(const char** it = commands; *it != NULL; it++) {
        printf("%s\n", *it);
    }
    print_bar(55);
}

void elf_menu(const char* filename) {
    static Elf64_Ehdr* elf_header = NULL;
    static char command_buffer[50] = {0};
    static int quit = 0;
    static size_t size = 0;

    elf_header = (Elf64_Ehdr*) elf_load(filename, &size);
    if(elf_header == NULL) {
        printf("Unable to open file.\n");
        return;
    }

    printf("type 'quit' to exit, or 'help' for commands.\n");
    while(quit == 0) {
        printf("\npatcher> ");
        char* it = fgets(command_buffer, sizeof(command_buffer), stdin);
        while(*it != '\n') { it++; };
        *it = '\0';

        char* arg0 = strtok(command_buffer, " ");
        arg0 = (arg0 == NULL) ? command_buffer : arg0;


        if(strcmp(command_buffer, "quit") == 0) {
            quit = 1;
            continue;
        }

        if(strcmp(command_buffer, "show") == 0) {
            char* arg1 = strtok(NULL, " ");
            if(arg1 == NULL) {
                elf_menu_print(elf_header);
                continue;
            }

            if(strcmp(arg1, "header") == 0) {
                elf_print_header(elf_header);
                continue;
            }

            if(strcmp(arg1, "section") == 0) {
                char* arg2 = strtok(NULL, " ");
                (arg2 == NULL)
                    ? elf_menu_print_all_sections(elf_header)
                    : elf_menu_print_section(elf_header, arg2);
                continue;
            }

            if(strcmp(arg1, "symbol") == 0) {
                char* arg2 = strtok(NULL, " ");

                (arg2 == NULL)
                    ? elf_menu_print_all_symbols(elf_header)
                    : elf_menu_print_symbol(elf_header, arg2);
                continue;
            }

            printf("expected header, section or symbol.\n");
            continue;
        }
        if(strcmp(command_buffer, "help") == 0) {
            elf_menu_help();
            continue;
        }
    }

    elf_free((char*)elf_header);
}

void elf_cli_menu(const char* filename, char* cli) {
    static Elf64_Ehdr* elf_header = NULL;
    static char command_buffer[50] = {0};
    static int quit = 0;

    elf_header = (Elf64_Ehdr*) elf_load(filename);
    if(elf_header == NULL) {
        printf("Unable to open file.\n");
        return;
    }

    switch(cli[0]) {
        case 'h':
            elf_menu_help();
            break;
        case 's':
            switch(cli[1]) {
                case 'h':
                    elf_print_header(elf_header);
                    break;
                case 'x':
                    (strlen(cli) > 2)
                    ? elf_menu_print_section(elf_header, cli + 2)
                    : elf_menu_print_all_sections(elf_header);
                    break;
                case 's':
                    (strlen(cli) > 2)
                    ? elf_menu_print_symbol(elf_header, cli + 2)
                    : elf_menu_print_all_symbols(elf_header);
                    break;
            }
            break;
        case 'p':
            break;
        case 'q':
            break;
    }

    elf_free((char*)elf_header);
}






