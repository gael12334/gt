/*
    Copyright © 2025 Gaël Fortier <gael.fortier.1@ens.etsmtl.ca>
*/

#include "elfpv.h"
#include "elfpv_buffer.h"
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
#define print_dbg() printf("%s %i\n", __FILE__, __LINE__)
#define bar_length 55
#define TEXT_NOP (0x90)
#define TEXT_ENDBR64_OFFSET (4)
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

static const uint8_t patch_template[] = {
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

static const char* elf_hijack_symbol_name;
static const char* elf_hook_symbol_name;
static size_t elf_file_size = 0;

void elf_set_symbol_name(const char* hijack, const char* hook) {
    elf_hijack_symbol_name = hijack;
    elf_hook_symbol_name = hook;
}

// ---- loading ----

static FILE* elf_open_file(const char* path, size_t* size) {
    FILE* f = fopen(path, "rb");
    if(f == NULL) { return NULL; }
    fseeko(f, 0, SEEK_END);
    *size = ftello(f);
    fseeko(f, 0, SEEK_SET);
    return f;
}

elfpv_buffer elf_load(const char* path, size_t* not_null_size) {
    elfpv_buffer buffer = {0};
    FILE* f = elf_open_file(path, not_null_size);
    if(!f) { return buffer; }

    buffer = elfpv_create_buffer(*not_null_size);
    if(elfpv_error_thrown()) { return buffer; }

    elfpv_buffer_value b;
    for(size_t i = 0; i < *not_null_size; i++) {
        fread(&b._u8, 1, 1, f);
        elfpv_write_buffer(buffer, i, ELFPV_BUFFER_VALUE_UINT8, b);
        if(elfpv_error_thrown()) {
            elfpv_print_buffer_error();
            return (elfpv_buffer) {0};
        }
    }

    fclose(f);
    return buffer;
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
    print_dbg();
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

uint8_t* elf_get_symbol_text(Elf64_Ehdr* header, Elf64_Sym* symbol) {
    size_t type = elf_get_symbol_type(header, symbol);
    if(type != STT_FUNC) { return NULL; }
    return (void*) header + symbol->st_value;
}

size_t elf_get_symbol_text_padding(Elf64_Sym* symbol, const uint8_t* text, size_t offset, uint8_t pad) {
    size_t i = offset;
    print_dbg();

    while(i < symbol->st_size && text[i] == pad) { i++; print_u64(i);}
    return i - offset;
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

void elf_print_symbol_text(Elf64_Ehdr* header, Elf64_Sym* symbol) {
    uint8_t* text = elf_get_symbol_text(header, symbol);
    if(text == NULL) { return; }

    char* name_list = elf_get_symbol_name_list(header, NULL);
    char* name = name_list + symbol->st_name;
    size_t start_offset = symbol->st_value;
    size_t end_offset = symbol->st_value + symbol->st_size;
    size_t padding = elf_get_symbol_text_padding(symbol, text, TEXT_ENDBR64_OFFSET, TEXT_NOP);

    printf("%s [%06zx : %06zx] padding %zu\n", name, start_offset, end_offset, padding);
    print_bar(bar_length);
    for(size_t i = 0; i < symbol->st_size; i += 8) {
        printf("%06zx : ", (void*)text + i - (void*) header);
        size_t limit = (symbol->st_size < i + 8) ? symbol->st_size : i + 8;
        for(size_t n = i; n < limit; n++) { printf("%02hhX ", text[n]); }
        printf("\n");
    }
    print_bar(bar_length);
}

// ---- patch ----

uint8_t* elf_get_text(Elf64_Ehdr* header, Elf64_Sym* symbol, size_t* not_null_size) {
    size_t type = elf_get_symbol_type(header, symbol);
    if(type >= STT_NUM) { return NULL; }

    *not_null_size = symbol->st_size;
    return (void*) header + symbol->st_value;
}

void elf_patch_text(Elf64_Ehdr* header, Elf64_Sym* symbol, Elf64_Sym* hijack, Elf64_Sym* hook) {
    print_dbg();
    size_t type = elf_get_symbol_type(header, symbol);
    size_t binding = elf_get_symbol_binding(header, symbol);

    if(type != STT_FUNC || binding != STB_GLOBAL) { return; }
    elf_print_symbol(header, symbol);
    if(symbol->st_value >= elf_file_size) { print_dbg(); return; }
    elf_print_symbol(header, symbol);

    print_dbg();
    uint8_t* text = elf_get_symbol_text(header, symbol);
    size_t padding = elf_get_symbol_text_padding(symbol, text, TEXT_ENDBR64_OFFSET, TEXT_NOP);
    if(padding < TEXT_PADDING) { return; }

    print_dbg();

    // offset of each instruction in patch_template.
    const size_t instr_offset[] = {
        4,
        instr_offset[0] + 1,
        instr_offset[1] + 7,
        instr_offset[2] + 7,
        instr_offset[3] + 3,
        instr_offset[4] + 6,
        instr_offset[5] + 1,
        instr_offset[6] + 3,
        instr_offset[7] + 7,
        instr_offset[8] + 7,
        instr_offset[9] + 2,
        instr_offset[10] + 1,
    };

    print_dbg();
    // offset of bytes to overwrite in instructions
    size_t instr_hijack_offset = instr_offset[1] + 3;
    size_t instr_hook_offset = instr_offset[7] + 3;

    print_dbg();
    // pointer to the data in memory of the hijack & hook symbols
    void* elf_hijack_ptr = ((void*)header + hijack->st_value);
    void* elf_hook_ptr = ((void*)header + hook->st_value);

    print_dbg();
    // pointer to the data in memory of the next instructions
    void* elf_hijack_next_instr_ptr = (text + instr_offset[2]);
    void* elf_hook_next_instr_ptr = (text + instr_offset[8]);

    print_dbg();
    // offset between location of symbols and following instructions.
    uint32_t hijack_offset = elf_hijack_ptr - elf_hijack_next_instr_ptr;
    uint32_t hook_offset = elf_hook_ptr - elf_hook_next_instr_ptr;

    print_dbg();
    // write operation
    memcpy(text + TEXT_ENDBR64_OFFSET, patch_template, sizeof(patch_template));
    memcpy(text + instr_hijack_offset, &hijack_offset, sizeof(hijack_offset));
    memcpy(text + instr_hook_offset, &hook_offset, sizeof(hook_offset));

    print_dbg();
    char* name_list = elf_get_symbol_name_list(header, NULL);
    char* symbol_name = name_list + symbol->st_name;
    size_t offset = (void*)text + TEXT_ENDBR64_OFFSET - (void*)header;
    printf("wrote %zu bytes at %06lX for symbol %s\n", sizeof(patch_template), offset, symbol_name);

    print_dbg();
}

void elf_patch_symbols(Elf64_Ehdr* header, Elf64_Sym* symbols, size_t number) {
    print_dbg();

    Elf64_Sym* hijack = elf_get_symbol_by_name(header, elf_hijack_symbol_name);
    Elf64_Sym* hook = elf_get_symbol_by_name(header, elf_hook_symbol_name);

    if(hijack == NULL || hook == NULL) {
        printf("Unable to patch : hijack and/or hook symbols are absent.\n");
        return;
    }

    for(size_t i = 0; i < number; i++) {
        elf_patch_text(header, symbols + i, hijack, hook);
    }

    print_dbg();
}

void elf_patch_symbol_by_name(Elf64_Ehdr* header, const char* name, size_t buffer_size) {
    Elf64_Sym* symbol = elf_get_symbol_by_name(header, name);

    if(symbol == NULL) {
        printf("Unable to patch : symbol %s is absent.\n", name);
        return;
    }

    elf_patch_symbols(header, symbol, 1);
    elf_save("out", (void*)header, buffer_size);
}

void elf_patch_all_symbols(Elf64_Ehdr* header, size_t buffer_size) {
    size_t number = 0;
    Elf64_Sym* symbols = elf_get_symbol_list(header, &number);

    if(symbols == NULL) {
        printf("Unable to patch : symbol list is empty.\n");
        return;
    }

    elf_patch_symbols(header, symbols, number);
    elf_save("out", (void*)header, buffer_size);
}

// ---- menu ----
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

void elf_menu_print_text(Elf64_Ehdr* header, char* arg2) {
    Elf64_Sym* symbol = elf_get_symbol_by_name(header, arg2);
    symbol == NULL ? printf("Invalid symbol name.\n") : elf_print_symbol_text(header, symbol);
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

    for(size_t i = 0; i < symbol_number; i++) {
        elf_print_symbol(header, symbol_list + i);
    }
}

void elf_menu_print_all_text(Elf64_Ehdr* header) {
    size_t symbol_number = 0;
    Elf64_Sym* symbol_list = elf_get_symbol_list(header, &symbol_number);

    for(size_t i = 0; i < symbol_number; i++) {
        elf_print_symbol_text(header, symbol_list + i);
    }
}

void elf_menu_print(Elf64_Ehdr* header) {
    elf_print_header(header);
    elf_menu_print_all_sections(header);
    elf_menu_print_all_symbols(header);
}

void elf_menu_help(void) {
    const char* commands[] = {
        "help             : shows this.",
        "show             : shows everything.",
        "  header         : shows the elf header.",
        "  section        : shows all the sections.",
        "    <name>       : shows section with specified name.",
        "    <index>      : shows section at specified index.",
        "  symbol         : shows all the symbols.",
        "    <name>       : shows symbol with specified name.",
        "    <index>      : shows symbol at specified index",
        "  text           : shows text segment of functions",
        "    <name>       : shows text segment for specified fn",
        "patch            : patches elf to hijack fn calls",
        "  <name>         : patches specified function",
        "quit             : returns to terminal.",
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
    static size_t elf_size = 0;

    elf_header = elf_load(filename, &elf_size).buffer;
    if(elf_header == NULL) {
        elfpv_buffer_error err = elfpv_get_error();
        elfpv_print_buffer_error();
        printf("Unable to open file.\n");
        return;
    }

    elf_file_size = elf_size;

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

            if(strcmp(arg1, "text") == 0) {
                char* arg2 = strtok(NULL, " ");

                (arg2 == NULL)
                    ? elf_menu_print_all_text(elf_header)
                    : elf_menu_print_text(elf_header, arg2);
            }

            printf("expected header, section or symbol.\n");
            continue;
        }

        if(strcmp(command_buffer, "patch") == 0) {
            char* arg1 = strtok(NULL, " ");
            (arg1 == NULL)
                ? elf_patch_all_symbols(elf_header, elf_size)
                : elf_patch_symbol_by_name(elf_header, arg1, elf_size);
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
    static size_t elf_size = 0;

    elf_header = elf_load(filename, &elf_size).buffer;
    if(elf_header == NULL) {
        printf("Unable to open file.\n");
        return;
    }

    print_u64(elf_size);
    elf_file_size = elf_size;

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
                case 't':
                    (strlen(cli) > 2)
                    ? elf_menu_print_text(elf_header, cli + 2)
                    : elf_menu_print_all_text(elf_header);
                    break;
            }
            break;
        case 'p':
            (strlen(cli) > 1)
            ? elf_patch_all_symbols(elf_header, elf_size)
            : elf_patch_symbol_by_name(elf_header, cli + 1, elf_size);
            break;
        case 'q':
            break;
    }

    elf_free((char*)elf_header);
}
