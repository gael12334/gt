/*
    Copyright (c) 2026 Gaël Fortier <gael.fortier.1@ens.etsmtl.ca>

    References:
        http://ref.x86asm.net/coder64.html#modrm_byte_32_64
        https://en.wikipedia.org/wiki/ModR/M
        https://en.wikipedia.org/wiki/REX_prefix

    Patch:
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
*/
#include "lib.h"
#include <elf.h>
#include <stdint.h>
#define ENDBR64 (0xfa1e0ff3)

typedef struct {
    Elf64_Sym* symbol;
    const char* name;
    size_t elf_offset;
    size_t name_length;
    size_t endbr64_offset;
    size_t available_padding;
    size_t instructions_size;
    const uint8_t* instructions;
    Elf64_Shdr* name_section_header;
    Elf64_Shdr* symbol_section_header;
} elf_function_info;

static int elf_patch_load_function(Elf64_Shdr* section_header, Elf64_Sym* symbol, elf_function_info* info)
{
    elf_check(elf_get_sym_strtab_shdr(section_header, &info->name_section_header));
    elf_check(elf_get_sym_offset(symbol, &info->elf_offset));
    elf_check(elf_get_strtab_shdr_text(info->name_section_header, symbol->st_name, &info->name));
    elf_check(elf_get_readonly_sym_bytes(symbol, &info->instructions, &info->instructions_size));
    info->symbol = symbol;
    info->symbol_section_header = section_header;
    info->available_padding = 0;
    info->name_length = strlen(info->name);

    const uint32_t endbr64 = ENDBR64;
    int has_endbr64 = (memcmp(info->instructions, &endbr64, sizeof(endbr64)) == 0);
    info->endbr64_offset = (has_endbr64) ? sizeof(endbr64) : 0;

    const uint8_t* byte = info->instructions + info->endbr64_offset;
    const uint8_t* end = info->instructions + info->instructions_size;
    while (byte != end && *byte == 0x90) { // 0x90 = NOP
        byte++;
    }

    info->available_padding = byte - (info->instructions + info->endbr64_offset);
    return elf_stack_error(ELF_OK);
}

static int elf_patch_elf_impl(void)
{
    elf_index_iterator it;
    elf_function_info fn_info;
    Elf64_Shdr* symbol_string_section_header;
    Elf64_Shdr* symbol_section_header;
    Elf64_Sym* hijack_symbol; // loaded symbols location
    Elf64_Sym* hook_symbol;
    size_t hijack_symbol_offset; // symbol offset in ELF file
    size_t hook_symbol_offset;
    int error;

    char divider[70] = { 0 };
    const size_t divider_length = 61;
    const size_t hijack_patch_offset = 4;
    const size_t hijack_nxinst_offset = hijack_patch_offset + sizeof(uint32_t);
    const size_t hook_patch_offset = 31;
    const size_t hook_nxinst_offset = hook_patch_offset + sizeof(uint32_t);
    uint8_t patch_bytes[] = {
        /*size  byte     comment */
        /* 1 */ 0x90, // NOP
        /*   */ /**/
        /* 1 */ 0x48, // REX
        /* 1 */ 0x8b, // MOV
        /* 1 */ 0x05, // MOD R/M
        /* 4 */ 0xFF, 0xFF, 0xFF, 0xFF, // 32 BIT OFFSET FROM NEXT INSTRUCTION TO BSS ADDRESS OF _gt_hjck
        /*   */ /**/
        /* 1 */ 0x4c, // REX
        /* 1 */ 0x8b, // MOV
        /* 1 */ 0x25, // MOD R/M
        /* 4 */ 0xed, 0xff, 0xff, 0xff, // 32 BIT OFFSET FROM NEXT INSTRUCTION TO START OF FUNCTION
        /*   */ /**/
        /* 1 */ 0x49, // REX
        /* 1 */ 0x3b, // CMP
        /* 1 */ 0xc4, // MOD R/M
        /*   */ /**/
        /* 1 */ 0x0f, // 2 BYTE OPCODE
        /* 1 */ 0x84, // JZ
        /* 4 */ 0x1a, 0x00, 0x00, 0x00, // IF ZERO, JUMP BY 0x1A BYTES
        /*   */ /**/
        /* 1 */ 0x55, // PUSH RBP
        /*   */ /**/
        /* 1 */ 0x48, // REX
        /* 1 */ 0x89, // MOV
        /* 1 */ 0xe5, // MOD R/M
        /*   */ /**/
        /* 1 */ 0x48, // REX
        /* 1 */ 0x8b, // MOV
        /* 1 */ 0x1d, // MOD R/M
        /* 4 */ 0xFF, 0xFF, 0xFF, 0xFF, // 32 BIT OFFSET FROM NEXT INSTRUCTION TO BSS ADDRESS OF _gt_hook
        /*   */ /**/
        /* 1 */ 0x48, // REX
        /* 1 */ 0xc7, // MOV
        /* 1 */ 0xc0, // MOD R/M
        /* 4 */ 0x00, 0x00, 0x00, 0x00, // 32 BIT VALUE MOVED IN REGISTER
        /*   */ /**/
        /* 1 */ 0xff, // CALL
        /* 1 */ 0xd3, // MOD R/M
        /*   */ /**/
        /* 1 */ 0x5d, // POP RBP
        /*   */ /**/
        /* 1 */ 0xc3, // RET
    };

    elf_check(elf_get_sym_shdr(&symbol_section_header));
    elf_check(elf_get_sym_wname(symbol_section_header, "_gt_hjck", &hijack_symbol));
    elf_check(elf_get_sym_wname(symbol_section_header, "_gt_hook", &hook_symbol));
    elf_check(elf_get_sym_offset(hijack_symbol, &hijack_symbol_offset));
    elf_check(elf_get_sym_offset(hook_symbol, &hook_symbol_offset));
    elf_check(elf_reset_index_iterator(&it));

    while ((error = elf_get_sym_wtype(symbol_section_header, STT_FUNC, &it)) == ELF_OK) {
        elf_check(elf_patch_load_function(symbol_section_header, it.object, &fn_info));

        size_t name_divider_index = (divider_length - fn_info.name_length) / 2;
        memset(divider, '=', divider_length);
        memcpy(&divider[name_divider_index], fn_info.name, fn_info.name_length);

        // elf offset of next instruction for hijack and hook symbols
        size_t elf_hijack_nxinst_offset = fn_info.elf_offset + hijack_nxinst_offset + fn_info.endbr64_offset;
        size_t elf_hook_nxinst_offset = fn_info.elf_offset + hook_nxinst_offset + fn_info.endbr64_offset;

        // distance from next instruction to elf offset of hijack and hook symbols
        uint32_t elf_hijack_func_distance = hijack_symbol_offset - elf_hijack_nxinst_offset;
        uint32_t elf_hook_func_distance = hook_symbol_offset - elf_hook_nxinst_offset;

        // pointer in patch for distance from next instruction to elf offset of hijack and hook symbols
        uint8_t* patch_hijack_bytes = &patch_bytes[hijack_patch_offset];
        uint8_t* patch_hook_bytes = &patch_bytes[hook_patch_offset];
        memcpy(patch_hijack_bytes, &elf_hijack_func_distance, sizeof(uint32_t));
        memcpy(patch_hook_bytes, &elf_hook_func_distance, sizeof(uint32_t));

        printf("\n%s\n", divider);
        printf("%-20s @ %zx\n", fn_info.name, fn_info.elf_offset);
        printf("%-20s @ %zx\n", "_gt_hjck", hijack_symbol_offset);
        printf("%-20s @ %zx\n", "_gt_hook", hook_symbol_offset);
        printf("%-20s @ %x\n", "diff hjck", elf_hijack_func_distance);
        printf("%-20s @ %x\n", "diff hook", elf_hook_func_distance);

        // WRITE PATCH
        elf_check(elf_set_sym_bytes(fn_info.symbol, fn_info.endbr64_offset, patch_bytes, sizeof(patch_bytes)));
        elf_check(elf_print_sym_bytes(fn_info.symbol, fn_info.instructions, fn_info.instructions_size));
    }

    return elf_stack_error(ELF_OK);
}

int elf_patch_elf(const char* file)
{
    elf_check(elf_load_elf(file));
    int error = elf_patch_elf_impl();
    printf("%i\n", error);
    if (!error) {
        elf_check(elf_save_elf(file));
    }
    elf_check(elf_unload_elf());
    return error;
}
