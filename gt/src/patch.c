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
#include "elf.h"
#include "gt2.h"
#include <elf.h>
#include <stdint.h>
#include <stdio.h>

const static uint8_t NOP = 0x90;
const static uint32_t ENDBR64 = 0xfa1e0ff3;

typedef struct {
    uint8_t i0_nop[1];
    uint8_t i1_mov_rax_qwordptr[3];
    uint8_t i1_offset32_hijack[4];
    uint8_t i2_mov_r12_qwordptr[3];
    uint8_t i2_offset32[4];
    uint8_t i3_cmp_rax_r12[3];
    uint8_t i4_jnz_near[2];
    uint8_t i4_offset32[4];
    uint8_t i5_push_rbp[1];
    uint8_t i6_mov_rbp_rsp[3];
    uint8_t i7_mov_rbx_qwordptr[3];
    uint8_t i7_offset32_mock[4];
    uint8_t i8_mov_rax_imm32[3];
    uint8_t i8_immediate32[4];
    uint8_t i9_call_rbx[2];
    uint8_t i10_pop_rbp[1];
    uint8_t i11_ret[1];
} elf_mock_patch_layout;

typedef struct {
    uint8_t i0_push_rbp[1];
    uint8_t i1_mov_rbp_rsp[3];
    uint8_t i3_call_near[1];
    uint8_t i3_offset32_main[4];
    uint8_t i4_leave[1];
    uint8_t i5_ret[1];
} elf_main_patch_layout;

typedef struct {
    size_t offset;
    size_t text_size;
    Elf64_Sym* symbol;
    Elf64_Shdr* section;
    elf_pcub8 text;
    const char* name;
    size_t name_length;
    Elf64_Shdr* name_section;
} elf_symbol_info;

typedef struct {
    elf_symbol_info symbol;
    size_t endbr64;
    size_t padding;
    int entrypoint;
} elf_function_info;

static elf_mock_patch_layout elf_mock_patch = {
    .i0_nop = { NOP },                                // 1    | NOP
    .i1_mov_rax_qwordptr = { 0x48, 0x8b, 0x05 },      // 3    | MOV RAX, qword ptr [offset32]
    .i1_offset32_hijack = { 0xff, 0xff, 0xff, 0xff }, // 4    | offset32 -> offset from IP to target function pointer
    .i2_mov_r12_qwordptr = { 0x4c, 0x8b, 0x25 },      // 3    | MOV R12, qword ptr [offset32]
    .i2_offset32 = { 0xed, 0xff, 0xff, 0xff },        // 4    | offset32 -> offset from IP to first byte of current function
    .i3_cmp_rax_r12 = { 0x49, 0x3b, 0xc4 },           // 3    | CMP RAX, R12
    .i4_jnz_near = { 0x0f, 0x85 },                    // 2    | JNZ offset32
    .i4_offset32 = { 0x1a, 0x00, 0x00, 0x00 },        // 4    | offset32 -> offset from IP to next instruction
    .i5_push_rbp = { 0x55 },                          // 1    | PUSH RBP
    .i6_mov_rbp_rsp = { 0x48, 0x89, 0xe5 },           // 3    | MOV RBP, RSP
    .i7_mov_rbx_qwordptr = { 0x48, 0x8b, 0x1d },      // 3    | MOV RBX, qword ptr [offset32]
    .i7_offset32_mock = { 0xff, 0xff, 0xff, 0xff },   // 4    | offset32 -> offset from IP to mock function pointer
    .i8_mov_rax_imm32 = { 0x48, 0xc7, 0xc0 },         // 3    | MOV RAX, immediate32
    .i8_immediate32 = { 0x00, 0x00, 0x00, 0x00 },     // 4    | immediate32 -> literal 32 bits integer value
    .i9_call_rbx = { 0xff, 0xd3 },                    // 2    | CALL RBX
    .i10_pop_rbp = { 0x5d },                          // 1    | POP RBP
    .i11_ret = { 0xc3 }                               // 1    | RET
};

static elf_main_patch_layout elf_main_patch = {
    .i0_push_rbp = { 0x55 },                        // 1    | PUSH RBP
    .i1_mov_rbp_rsp = { 0x48, 0x89, 0xe5 },         // 3    | MOV RBP, RSP
    .i3_call_near = { 0xe8 },                       // 1    | CALL (near)
    .i3_offset32_main = { 0xff, 0xff, 0xff, 0xff }, // 4    | 32 bit offset of next instruction to 1st byte of gt_test_main
    .i4_leave = { 0xc9 },                           // 1    | LEAVE
    .i5_ret = { 0xc3 }                              // 1    | RET
};

static int elf_check_elf_type(void)
{
    Elf64_Ehdr* elf_header;
    elf_get_hdr(&elf_header);

    switch (elf_header->e_type) {
    case ET_EXEC:
    case ET_REL:
    case ET_DYN:
        return elf_stack_error(ELF_OK);
    default:
        printf("Unsupported ELF type. (%hu)\n", elf_header->e_type);
        return elf_stack_error(ELF_ERR_TYPE);
    }
}

static void elf_print_divider(const char* name, size_t length)
{
    static char divider[64] = { 0 };
    unsigned writeable = 60;
    unsigned spread = (writeable - length) / 2;

    puts("");
    memset(divider, '=', writeable);
    divider[writeable] = '\0';
    divider[spread - 1] = ' ';
    divider[spread + length] = ' ';
    memcpy(&divider[spread], name, length);
    printf("%s\n", divider);
}

int elf_load_symbol_info(Elf64_Shdr* section, Elf64_Sym* symbol, elf_symbol_info* info_ref)
{
    if (!section || !symbol || !info_ref) {
        return elf_stack_error(ELF_ERR_NULL);
    }

    elf_check(elf_get_sym_offset(symbol, &info_ref->offset));
    elf_check(elf_get_readonly_sym_bytes(symbol, &info_ref->text, &info_ref->text_size));
    elf_check(elf_get_sym_strtab_shdr(section, &info_ref->name_section));
    elf_check(elf_get_strtab_shdr_text(info_ref->name_section, symbol->st_name, &info_ref->name));
    info_ref->name_length = strnlen(info_ref->name, info_ref->name_section->sh_size);
    info_ref->symbol = symbol;
    info_ref->section = section;
    return elf_stack_error(ELF_OK);
}

static int elf_load_function_info(Elf64_Shdr* section, Elf64_Sym* symbol, elf_function_info* info_ref)
{
    if (!section || !symbol || !info_ref) {
        return elf_stack_error(ELF_ERR_NULL);
    }

    elf_check(elf_load_symbol_info(section, symbol, &info_ref->symbol));

    // check if function starts with ENDBR64
    const uint8_t* text = info_ref->symbol.text;
    const uint32_t* text_endbr64 = (const uint32_t*)text;
    info_ref->endbr64 = (*text_endbr64 == ENDBR64) ? sizeof(ENDBR64) : 0;
    text += info_ref->endbr64;

    // counts padding
    info_ref->padding = 0;
    size_t* padding = &info_ref->padding;
    const size_t text_size = info_ref->symbol.text_size;
    while (text[*padding] == NOP && *padding < text_size) {
        (*padding)++;
    }

    return elf_stack_error(ELF_OK);
}

static int elf_patch_function(elf_function_info* function, elf_symbol_info* hijack, elf_symbol_info* mock)
{
    static uint8_t* patch_start = (uint8_t*)&elf_mock_patch.i0_nop[0];
    static int32_t* hijack_ptr = (int32_t*)&elf_mock_patch.i1_offset32_hijack[0];
    static int32_t* mock_ptr = (int32_t*)&elf_mock_patch.i7_offset32_mock[0];

    if (function->padding < 50) {
        printf("Padding too small (%lu bytes) to patch function.\n", function->padding);
        return elf_stack_error(ELF_OK);
    }

    int32_t offset;

    offset = function->symbol.offset + function->endbr64 + (uint8_t*)(hijack_ptr + 1) - (uint8_t*)&elf_mock_patch;
    *hijack_ptr = hijack->offset - offset;

    offset = function->symbol.offset + function->endbr64 + (uint8_t*)(mock_ptr + 1) - (uint8_t*)&elf_mock_patch;
    *mock_ptr = mock->offset - offset;

    elf_check(elf_set_sym_bytes(function->symbol.symbol, function->endbr64, (uint8_t*)&elf_mock_patch, sizeof(elf_mock_patch)));
    return elf_stack_error(ELF_OK);
}

static int elf_patch_entrypoint(elf_function_info* entry, elf_function_info* test_entry)
{
    static uint8_t* patch_start = (uint8_t*)&elf_main_patch.i0_push_rbp[0];
    static int32_t* test_entry_ptr = (int32_t*)&elf_main_patch.i3_offset32_main[0];

    int32_t offset = entry->symbol.offset + entry->endbr64 + (uint8_t*)(test_entry_ptr + 1) - (uint8_t*)patch_start;
    *test_entry_ptr = test_entry->symbol.offset - offset;

    elf_check(elf_set_sym_bytes(entry->symbol.symbol, entry->endbr64, (uint8_t*)&elf_main_patch, sizeof(elf_main_patch)));
    return elf_stack_error(ELF_OK);
}

static int elf_patch_elf_impl(const char* entrypoint, int verbose)
{
    elf_check(elf_check_elf_type());

    Elf64_Shdr* section;
    Elf64_Sym* hijack_sym;
    Elf64_Sym* mock_sym;
    Elf64_Sym* gtmain_sym;
    Elf64_Sym* entry_sym;
    elf_index_iterator it;

    elf_check(elf_get_sym_shdr(&section));
    elf_check(elf_get_sym_wname(section, gt_function_hijack_symbol_name(), &hijack_sym));
    elf_check(elf_get_sym_wname(section, gt_function_mock_symbol_name(), &mock_sym));
    elf_check(elf_get_sym_wname(section, gt_function_main_symbol_name(), &gtmain_sym));
    elf_check(elf_get_sym_wname(section, entrypoint, &entry_sym));

    elf_symbol_info hijack = { 0 };
    elf_symbol_info mock = { 0 };
    elf_function_info entryptn = { 0 };
    elf_function_info gtmain = { 0 };
    elf_function_info func = { 0 };

    elf_check(elf_load_symbol_info(section, hijack_sym, &hijack));
    elf_check(elf_load_symbol_info(section, mock_sym, &mock));
    elf_check(elf_load_function_info(section, gtmain_sym, &gtmain));
    elf_check(elf_load_function_info(section, entry_sym, &entryptn));
    elf_check(elf_reset_index_iterator(&it));

    // patch entry point
    elf_check(elf_patch_entrypoint(&entryptn, &gtmain));

    int skipped = 0;
    int patched = 0;
    int error;
    while ((error = elf_get_sym_wtype(section, STT_FUNC, &it)) == ELF_OK) {
        elf_check(elf_load_function_info(section, it.object, &func));

        if (verbose) {
            elf_print_divider(func.symbol.name, func.symbol.name_length);
        }

        if (strcmp(gtmain.symbol.name, func.symbol.name) == 0) {
            if (verbose) {
                puts("test entry point.");
            }
        } else if (strcmp(entryptn.symbol.name, func.symbol.name) == 0) {
            if (verbose) {
                puts("program entry point.");
            }
        } else if (func.padding < 50) {
            if (verbose) {
                puts("padding size less than 50 bytes; skipping.");
            }
            skipped++;
        } else {
            elf_check(elf_patch_function(&func, &hijack, &mock));
            if (verbose) {
                printf("wrote %zu bytes at offset %zx.\n", sizeof(elf_mock_patch), func.symbol.offset + func.endbr64);
            }
            patched++;
        }

        //     // elf offset of next instruction for hijack and hook symbols
        //     size_t elf_hijack_nxinst_offset
        //     = +fn_info.endbr64_offset;
        // size_t elf_hook_nxinst_offset = fn_info.elf_offset + hook_nxinst_offset + fn_info.endbr64_offset;
        //
        // // distance from next instruction to elf offset of hijack and hook symbols
        // uint32_t elf_hijack_func_distance = hijack_symbol_offset - elf_hijack_nxinst_offset;
        // uint32_t elf_hook_func_distance = hook_symbol_offset - elf_hook_nxinst_offset;

        // pointer in patch for distance from next instruction to elf offset of hijack and hook symbols
        // uint8_t* patch_hijack_bytes = &elf_mock_patch[hijack_patch_offset];
        // uint8_t* patch_hook_bytes = &elf_mock_patch[hook_patch_offset];
        // memcpy(patch_hijack_bytes, &elf_hijack_func_distance, sizeof(uint32_t));
        // memcpy(patch_hook_bytes, &elf_hook_func_distance, sizeof(uint32_t));
        //
        // // WRITE PATCH
        // elf_check(elf_set_sym_bytes(fn_info.symbol, fn_info.endbr64_offset, elf_mock_patch, sizeof(elf_mock_patch)));
        // elf_check(elf_print_sym_bytes(fn_info.symbol, fn_info.instructions, fn_info.instructions_size));
    }

    // set initial mock to gt_test_main, to hijack main and
    // redirect execution to gt_test_main.
    // if (main_info.symbol && test_main_info.symbol) {
    //
    //     size_t elf_main_nxinst_offset = main_info.elf_offset + main_nxinst_offset + main_info.endbr64_offset;
    //     uint32_t elf_main_func_distance = test_main_info.elf_offset - elf_main_nxinst_offset;
    //     uint8_t* patch_main_bytes = &elf_main_patch[]
    //
    //                                 elf_check(elf_set_sym_bytes(hijack_symbol, 0, (elf_bytebuffer)&main_offset, sizeof(main_offset)));
    //     elf_check(elf_set_sym_bytes(hook_symbol, 0, (elf_bytebuffer)&test_main_offset, sizeof(test_main_offset)));
    //
    //     elf_update_divider(divider, divider_length, "hijack", sizeof("hijack") - 1);
    //     elf_print_sym_bytes_2(hijack_symbol);
    //     elf_update_divider(divider, divider_length, "hook", sizeof("hook") - 1);
    //     elf_print_sym_bytes_2(hook_symbol);
    // }

    printf("\n\nPatched: %i\nSkipped: %i\n", patched, skipped);
    return elf_stack_error(ELF_OK);
}

int elf_patch_elf(const char* entrypoint, int verbose)
{
    elf_check(elf_patch_elf_impl(entrypoint, verbose));
    return elf_stack_error(ELF_OK);
}
