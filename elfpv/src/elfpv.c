/*
    Copyright (c) 2025 Gaël Fortier <gael.fortier.1@ens.etsmtl.ca>
*/

/* References
 * https://www.eevblog.com/forum/programming/st-cube-gcc-how-to-override-a-function-not-defined-as-weak-(no-sources)/
 * https://sourceware.org/binutils/docs/binutils/objcopy.html
 * https://www.tutorialspoint.com/c_standard_library/c_function_fseek.htm
 * https://man7.org/linux/man-pages/man5/elf.5.html
 * https://dwarfstd.org/dwarf5std.html
 * https://www.felixcloutier.com/x86/push
 * web.stanford.edu/class/archive/cs/cs107/cs107.1202/guide/x86-64.html
 * https://www.cs.uaf.edu/2016/fall/cs301/lecture/09_28_machinecode.html
 * https://simplifycpp.org/?id=a0740
 * https://en.wikipedia.org/wiki/ModR/M
 * https://www.cs.princeton.edu/courses/archive/fall01/cs217/assignment5/2.1.html
 */

// static const uint8_t patch_template[] = {
//         0x90,                                       // nop
//         0x48, 0x8b, 0x05, 0xFF, 0xFF, 0xFF, 0xFF,   // mov rax, qword ptr [hijack]    ; offset from next instruction to hijack symbol
//         0x4c, 0x8b, 0x25, 0xed, 0xff, 0xff, 0xff,   // mov r12, qword ptr 0xedffffff  ; offset from next instruction to endbr64
//         0x49, 0x3b, 0xc4,                           // cmp rax, r12
//         0x0f, 0x84, 0x1a, 0x00, 0x00, 0x00,         // jz 0x1a                        ; jumps to start of function, after padding
//         0x55,                                       // push rbp
//         0x48, 0x89, 0xe5,                           // mov rbp, rsp
//         0x48, 0x8b, 0x1d, 0xFF, 0xFF, 0xFF, 0xFF,   // mov rbx, qword ptr [hook]      ; offset from next instruction to hook symbol
//         0x48, 0xc7, 0xc0, 0x00, 0x00, 0x00, 0x00,   // mov rax, 0x0
//         0xff, 0xd3,                                 // call rbx
//         0x5d,                                       // pop rbp
//         0xc3                                        // ret
// };

#include <assert.h>
#include <elf.h>
#include <linux/limits.h>
#include <stdio.h>

#include "elfpv_lib.h"
#include "elfpv_utils.h"

#if 1 // get commands
int get_hdr_command(int argc, char** args)
{
    elfpv_check(elfpv_print_header());
    return elfpv_stack_error(ELFPV_OK);
}

int get_phdr_command(int argc, char** args)
{
    if (argc < 1) {
        return elfpv_stack_error(ELFPV_ERR_ARGC);
    }

    long index;
    elfpv_check(elfpv_text_isnum(args[0], &index));

    Elf64_Phdr* ph;
    elfpv_check(elfpv_get_phdr_windex(index, &ph));
    elfpv_check(elfpv_print_phdr(ph));
    return elfpv_stack_error(ELFPV_OK);
}

int get_shdr_command(int argc, char** args)
{
    if (argc < 1) {
        return elfpv_stack_error(ELFPV_ERR_ARGC);
    }

    long index;
    elfpv_check(elfpv_text_isnum(args[0], &index));

    Elf64_Shdr* sh;
    elfpv_check(elfpv_get_shdr_windex(index, &sh));
    elfpv_check(elfpv_print_shdr(sh));
    return elfpv_stack_error(ELFPV_OK);
}

int get_sym_command(int argc, char** args)
{
    if (argc < 1) {
        return elfpv_stack_error(ELFPV_ERR_ARGC);
    }

    long index;
    elfpv_check(elfpv_text_isnum(args[0], &index));

    Elf64_Shdr* sym_sh;
    elfpv_check(elfpv_get_sym_shdr(&sym_sh));

    Elf64_Sym* sym;
    elfpv_check(elfpv_get_sym_windex(sym_sh, index, &sym));
    elfpv_check(elfpv_print_sym(sym_sh, sym));
    return elfpv_stack_error(ELFPV_OK);
}

int get_elf_command(int argc, char** args)
{
    elfpv_check(elfpv_print_elf());
    return elfpv_stack_error(ELFPV_OK);
}
#endif

#if 1 // get all commands
int get_all_shstr_command(int argc, char** args)
{
    Elf64_Shdr* shstrtab_sh;
    elfpv_check(elfpv_get_shdr_strtab_shdr(&shstrtab_sh));
    elfpv_check(elfpv_print_strtab_shdr_text(shstrtab_sh));
    return elfpv_stack_error(ELFPV_OK);
}

int get_all_str_command(int argc, char** args)
{
    Elf64_Shdr* strtab_sh;
    elfpv_check(elfpv_get_shdr_wname(".strtab", &strtab_sh));
    elfpv_check(elfpv_print_strtab_shdr_text(strtab_sh));
    return elfpv_stack_error(ELFPV_OK);
}

int get_all_sym_command(int argc, char** args)
{
    Elf64_Shdr* sym_sh;
    elfpv_check(elfpv_get_sym_shdr(&sym_sh));
    elfpv_check(elfpv_print_sym_shdr(sym_sh));
    return elfpv_stack_error(ELFPV_OK);
}

int get_all_sym_wtype_command(int type)
{
    Elf64_Shdr* sym_sh;
    elfpv_check(elfpv_get_sym_shdr(&sym_sh));

    int64_t index = 0;
    int64_t prev = -1;
    while (index != -1) {
        elfpv_check(elfpv_get_sym_wtype(sym_sh, type, prev, &index));

        if (index != -1) {
            Elf64_Sym* sym;
            elfpv_check(elfpv_get_sym_windex(sym_sh, index, &sym));
            elfpv_check(elfpv_print_sym(sym_sh, sym));
        }

        prev = index;
    }

    return elfpv_stack_error(ELFPV_OK);
}

int get_all_obj_command(int argc, char** args)
{
    elfpv_check(get_all_sym_wtype_command(STT_OBJECT));
    return elfpv_stack_error(ELFPV_OK);
}

int get_all_func_command(int argc, char** args)
{
    elfpv_check(get_all_sym_wtype_command(STT_FUNC));
    return elfpv_stack_error(ELFPV_OK);
}

#endif

#if 1 // find commands
int find_shdr_command(int argc, char** args)
{
    if (argc < 1) {
        return elfpv_stack_error(ELFPV_ERR_ARGC);
    }

    const char* name = args[0];
    Elf64_Shdr* shdr;
    elfpv_check(elfpv_get_shdr_wname(name, &shdr));
    elfpv_check(elfpv_print_shdr(shdr));
    return elfpv_stack_error(ELFPV_ERR_NOT_FOUND);
}

int find_sym_command(int argc, char** args)
{
    if (argc < 1) {
        return elfpv_stack_error(ELFPV_ERR_ARGC);
    }

    Elf64_Shdr* sym_sh;
    elfpv_check(elfpv_get_sym_shdr(&sym_sh));

    const char* name = args[0];
    Elf64_Sym* sym;
    elfpv_check(elfpv_get_sym_wname(sym_sh, name, &sym));
    elfpv_print_sym(sym_sh, sym);
    return elfpv_stack_error(ELFPV_ERR_NOT_FOUND);
}

int find_sym_command_wtype_command(int type, const char* name)
{
    Elf64_Shdr* sym_sh;
    elfpv_check(elfpv_get_sym_shdr(&sym_sh));

    Elf64_Shdr* symstrtab_sh;
    elfpv_check(elfpv_get_sym_strtab_shdr(sym_sh, &symstrtab_sh));

    int64_t index = 0;
    int64_t prev = -1;
    while (index != -1) {
        elfpv_check(elfpv_get_sym_wtype(sym_sh, type, prev, &index));

        if (index == -1) {
            prev = index;
            continue;
        }

        Elf64_Sym* sym;
        elfpv_check(elfpv_get_sym_windex(sym_sh, index, &sym));

        const char* sym_name;
        elfpv_check(elfpv_get_strtab_shdr_text(symstrtab_sh, sym->st_name, &sym_name));

        if (strcmp(sym_name, name) == 0) {
            elfpv_check(elfpv_print_sym(sym_sh, sym));
            return elfpv_stack_error(ELFPV_OK);
        }

        prev = index;
    }

    return elfpv_stack_error(ELFPV_ERR_NOT_FOUND);
}

int find_obj_command(int argc, char** args)
{
    if (argc < 1) {
        return elfpv_stack_error(ELFPV_ERR_ARGC);
    }

    elfpv_check(find_sym_command_wtype_command(STT_OBJECT, args[0]));
    return elfpv_stack_error(ELFPV_OK);
}

int find_func_command(int argc, char** args)
{
    if (argc < 1) {
        return elfpv_stack_error(ELFPV_ERR_ARGC);
    }

    elfpv_check(find_sym_command_wtype_command(STT_FUNC, args[0]));
    return elfpv_stack_error(ELFPV_OK);
}
#endif

#if 1 // print
int print_wname_command(const char* name)
{
    Elf64_Shdr* sym_sh;
    elfpv_check(elfpv_get_sym_shdr(&sym_sh));

    Elf64_Sym* sym;
    elfpv_check(elfpv_get_sym_wname(sym_sh, name, &sym));

    elfpv_constbytebuffer bytes;
    size_t size;
    elfpv_check(elfpv_get_readonly_sym_bytes(sym, &bytes, &size));

    elfpv_check(elfpv_print_sym_bytes(sym, bytes, size));
    return elfpv_stack_error(ELFPV_OK);
}

int print_text_command(int argc, char** args)
{
    if (argc < 1) {
        return elfpv_stack_error(ELFPV_ERR_ARGC);
    }

    elfpv_check(print_wname_command(args[0]));
    return elfpv_stack_error(ELFPV_OK);
}

int print_obj_command(int argc, char** args)
{
    if (argc < 1) {
        return elfpv_stack_error(ELFPV_ERR_ARGC);
    }

    elfpv_check(print_wname_command(args[0]));
    return elfpv_stack_error(ELFPV_OK);
}
#endif

#if 1 // count
int count_shdr_command(int argc, char** args)
{
    Elf64_Ehdr* hdr;
    elfpv_check(elfpv_get_hdr(&hdr));
    printf("shdr %hu\n", hdr->e_shnum);
    return elfpv_stack_error(ELFPV_OK);
}

int count_sym_command(int argc, char** args)
{
    Elf64_Shdr* sym_sh;
    elfpv_check(elfpv_get_sym_shdr(&sym_sh));

    size_t num;
    elfpv_check(elfpv_get_sym_num(sym_sh, &num));

    printf("sym %zu\n", num);
    return elfpv_stack_error(ELFPV_OK);
}

int count_func_wtype_command(uint16_t type)
{
    Elf64_Shdr* sym_sh;
    elfpv_check(elfpv_get_sym_shdr(&sym_sh));

    int64_t prev = -1;
    int64_t index = 0;
    size_t count = 0;

    while (index != -1) {
        elfpv_check(elfpv_get_sym_wtype(sym_sh, type, prev, &index));
        if (index != -1) {
            count++;
        }
        prev = index;
    }

    printf("sym %zu\n", count);
    return elfpv_stack_error(ELFPV_OK);
}

int count_func_command(int argc, char** args)
{
    elfpv_check(count_func_wtype_command(STT_FUNC));
    return elfpv_stack_error(ELFPV_OK);
}

int count_obj_command(int argc, char** args)
{
    elfpv_check(count_func_wtype_command(STT_OBJECT));
    return elfpv_stack_error(ELFPV_OK);
}
#endif

#if 1 // patch
int patch_sym_wtype_command(uint16_t type, const char* name, long offset, int argc, char** argv)
{
    if (offset < 0) {
        return elfpv_stack_error(ELFPV_ERR_OFFSET);
    }

    uint8_t* buffer = malloc(argc);
    if (buffer == NULL) {
        return elfpv_stack_error(ELFPV_ERR_MALLOC);
    }

    for (size_t i = 0; i < argc; i++) {
        int result = elfpv_hexstr_tobyte(argv[i], buffer + i);
        if (result) {
            printf("error at byte %zu\n", i);
            free(buffer);
            return result;
        }
    }

    Elf64_Shdr* sym_sh;
    elfpv_check(elfpv_get_sym_shdr(&sym_sh));

    Elf64_Sym* sym;
    elfpv_check(elfpv_get_sym_wname(sym_sh, name, &sym));
    elfpv_check(elfpv_set_sym_bytes(sym, (size_t)offset, buffer, argc));

    free(buffer);
    return elfpv_stack_error(ELFPV_OK);
}

int patch_func_command(int argc, char** args)
{
    if (argc < 2) {
        return elfpv_stack_error(ELFPV_ERR_ARGC);
    }

    long offset;
    elfpv_check(elfpv_text_isnum(args[1], &offset));

    elfpv_check(patch_sym_wtype_command(STT_FUNC, args[0], offset, argc - 1, args + 1));
    return elfpv_stack_error(ELFPV_OK);
}

int patch_obj_command(int argc, char** args)
{
    if (argc < 2) {
        return elfpv_stack_error(ELFPV_ERR_ARGC);
    }

    long offset;
    elfpv_check(elfpv_text_isnum(args[1], &offset));

    elfpv_check(patch_sym_wtype_command(STT_OBJECT, args[0], offset, argc - 1, args + 1));
    return elfpv_stack_error(ELFPV_OK);
}

#endif

#if 1 // offsetof

int offsetof_sym_wtype_command(const char* name)
{
    Elf64_Shdr* sym_sh;
    elfpv_check(elfpv_get_sym_shdr(&sym_sh));

    Elf64_Sym* sym;
    elfpv_check(elfpv_get_sym_wname(sym_sh, name, &sym));

    size_t offset;
    elfpv_check(elfpv_get_sym_offset(sym, &offset));

    printf("offset of %s is %zu (0x%zX)\n", name, offset, offset);
    return elfpv_stack_error(ELFPV_OK);
}

int offsetof_func_command(int argc, char** args)
{
    if (argc < 1) {
        return elfpv_stack_error(ELFPV_ERR_ARGC);
    }

    elfpv_check(offsetof_sym_wtype_command(args[0]));
    return elfpv_stack_error(ELFPV_OK);
}

int offsetof_obj_command(int argc, char** args)
{
    if (argc < 1) {
        return elfpv_stack_error(ELFPV_ERR_ARGC);
    }

    elfpv_check(offsetof_sym_wtype_command(args[0]));
    return elfpv_stack_error(ELFPV_OK);
}

#endif

#if 1 // commands
int load_command(int argc, char** args)
{
    // syntax: load <filename>
    if (argc < 1) {
        return elfpv_stack_error(ELFPV_ERR_ARGC);
    }

    elfpv_check(elfpv_load_elf(args[0]));
    return elfpv_stack_error(ELFPV_OK);
}

int unload_command(int argc, char** args)
{
    elfpv_check(elfpv_unload_elf());
    return elfpv_stack_error(ELFPV_OK);
}

int get_command(int argc, char** args)
{
    static elfpv_cmdent cmds0[] = {
        { "hdr", get_hdr_command }, // hdr
        { "phdr", get_phdr_command }, // phdr <index>
        { "shdr", get_shdr_command }, // shdr <index>
        { "elf", get_elf_command }, // elf
        { "sym", get_sym_command }, // sym <index>
    };

    int result = elfpv_execute(argc, args, cmds0, sizeof(cmds0) / sizeof(cmds0[0]));
    return elfpv_stack_error(result);
}

int get_all_command(int argc, char** args)
{
    static elfpv_cmdent cmds0[] = {
        { "sym", get_all_sym_command }, // sym
        { "str", get_all_str_command }, // str
        { "shstr", get_all_shstr_command }, // shstr
        { "func", get_all_func_command }, // func
        { "obj", get_all_obj_command }, // obj
    };

    int result = elfpv_execute(argc, args, cmds0, sizeof(cmds0) / sizeof(cmds0[0]));
    return elfpv_stack_error(result);
}

int find_command(int argc, char** args)
{
    static elfpv_cmdent cmds0[] = {
        { "shdr", find_shdr_command }, // shdr <name>
        { "sym", find_sym_command }, // sym <name>
        { "func", find_func_command }, // func <name>
        { "obj", find_obj_command }, // obj <name>
    };

    int result = elfpv_execute(argc, args, cmds0, sizeof(cmds0) / sizeof(cmds0[0]));
    return elfpv_stack_error(result);
}

int print_command(int argc, char** args)
{
    static elfpv_cmdent cmds0[] = {
        { "func", print_text_command }, // func <name>
        { "obj", print_obj_command }, // obj <name>
    };

    int result = elfpv_execute(argc, args, cmds0, sizeof(cmds0) / sizeof(cmds0[0]));
    return elfpv_stack_error(result);
}

int count_command(int argc, char** args)
{
    static elfpv_cmdent cmds0[] = {
        { "shdr", count_shdr_command }, // shdr
        { "sym", count_sym_command }, // sym
        { "func", count_func_command }, // func
        { "obj", count_obj_command }, // obj
    };

    int result = elfpv_execute(argc, args, cmds0, sizeof(cmds0) / sizeof(cmds0[0]));
    return elfpv_stack_error(result);
}

int patch_command(int argc, char** args)
{
    static elfpv_cmdent cmds0[] = {
        { "func", patch_func_command }, // func <name> <data...>
        { "obj", patch_obj_command }, // obj <name> <data...>
    };

    int result = elfpv_execute(argc, args, cmds0, sizeof(cmds0) / sizeof(cmds0[0]));
    return elfpv_stack_error(result);
}

int quit_command(int argc, char** args)
{
    if (elfpv_elf_loaded()) {
        unload_command(argc, args);
    }

    return elfpv_stack_error(ELFPV_QUIT);
}

int offsetof_command(int argc, char** args)
{
    static elfpv_cmdent cmds0[] = {
        { "func", offsetof_func_command }, // func <name>
        { "obj", offsetof_obj_command }, // obj <name>
    };

    int result = elfpv_execute(argc, args, cmds0, sizeof(cmds0) / sizeof(cmds0[0]));
    return elfpv_stack_error(result);
}

int get_error_command(int argc, char** args)
{
    elfpv_print_error_trace();
    return ELFPV_OK; // keeps error.
}
#endif

int elfpv_cli(void)
{
    static elfpv_cmdent cmds0[] = {
        { "load", load_command },
        { "get", get_command },
        { "get-all", get_all_command },
        { "patch", patch_command },
        { "unload", unload_command },
        { "quit", quit_command },
        { "find", find_command },
        { "print", print_command },
        { "count", count_command },
        { "offset-of", offsetof_command },
        { "get-error", get_error_command },
        { "exit", quit_command }
    };

    const int input_size = 255;
    const int tokens_size = 127;
    char input[input_size];
    char* tokens[tokens_size];
    int tok_num = 0;
    int result = ELFPV_OK;
    int argc;

    while (result != ELFPV_QUIT) {
        printf("> ");
        char* buf = fgets(input, input_size, stdin);
        putc('\n', stdout);
        elfpv_tokenize(buf, tokens_size, tokens, &argc);
        elfpv_reset_error_trace();
        result = elfpv_execute(argc, tokens, cmds0, sizeof(cmds0) / sizeof(cmds0[0]));
        putc('\n', stdout);
        if (result > 1) {
            printf("result %i\n\n", result);
        }
    }

    elfpv_unload_elf();
    return 0;
}

int main(int argc, char** argv)
{
    return elfpv_cli();
}
