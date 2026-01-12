<!--
    Copyright © 2026 Gaël Fortier <gael.fortier.1@ens.etsmtl.ca>
-->

Copyright © 2026 Gaël Fortier 

<gael.fortier.1@ens.etsmtl.ca>

## Disclaimer

This is a work in progress. 

This is intended to be used for my own personal projects and as a portfolio. 

Do not use this in a professional setting. 

Do not sell, distribute or copy. 

It works on my machine, but you should not expect that it works on yours. It is likely going to SEGFAULT somewhere. 

## GT (Gee-Tee)

Long story short, GT is a library and a tool used for unit testing in C codebases. GT was created because I wanted to write unit tests and functions mocks for my C projects. I designed this tool in a way that tests can be written and ran without ever touching the source code. For the moment, only ELF executable are supported but I plan on adding support for PE executable. This project is built using bash scripts and GCC on linux, and only uses Linux headers and `libc`.

### How it works

There are a few prerequisite to make this work, but the general idea is to generate an executable file that has patchable function entries and is compiled with GT's object file `gt.o`. With a patchable executable file, the GT command line tool (`cli.elf`) can generate a new executable file with the needed instructions in every functions that has sufficient padding (at least 50 bytes). For the main function, the patching tool will use the padding to redirect the control flow to GT's test entrypoint symbol (`gt_test_main`). 

The instructions pasted in the patchable entry of a function is needed to detect whether a function is mocked or not. Essentially, if the global pointer `gt_hijack` has the address of the current function, the mock pointed by `gt_mock` will be called and the value returned by the mock is the value that the function will return. A small demo project is available in [gt/demo](gt/demo).

In [gt/src/patch.c](gt/src/patch.c): 
```
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
```

The instructions above roughly translate to this: 
```
    if (this_function == gt_hijack) {
        void(*mock)() = gt_mock;
        mock();
        return;
    }
```

To patch an executable, run `cli.elf elf <filename> patch <entrypoint name>`. Note: in most cases, `entrypoint name` is the `main` function. Putting `_start` will not work.

### Prerequisite

- A 64 bit PC
- Linux or any OS using ELFs (for the moment)
- GCC 
- GT's `cli.elf` and `gt.o`

### How to build, patch and run

1. Open a terminal, then `cd` into the `gt` directory. 
2. Run `./build.sh all` to compile everything (including the demo). 
3. Run `./build.sh test` to patch `demo.elf` and run the generated executable `out`. 

### What you should exepect

The output of the demo project [gt/demo](gt/demo):

![](imgs/gt.png)

### Documentation

#### Build script

Syntax: `./build.sh <option>`

Options: 

- `cli` : builds `cli.elf`
- `gt` : builds `gt.o`
- `demo` : builds `gt.o` if it does not exists, then builds `demo.elf`
- `lib` : makes an archive (`libgt.a`) of `gt.o`. 
- `all` : builds everything.
- `test` : builds `cli.elf` if it does not exists, builds `gt.o`, `demo.elf`, runs the cli's patcher, then executes the test executable (`out`).

#### Command line app

Syntax: `./cli.elf <filename> <option>`

Options: 

- `header` : shows the elf's header info. 
- `funclist` : shows the list of symbols with type `STT_FUNC`.
- `func <name>` : shows the function symbol `<name>. `
- `patch [verbose]` : patches the elf file. `verbose` is optional. 
- `program` : shows the elf's program header array. 
- `section` : shows the elf's section header array.

#### Library (gt2.h & gt.o)

`gt_test_function(name)`

Macro. Required to write a test. Specifies the function currently tested. 

`gt_test_case(name)`

Macro. Required to write a test. Specifies the case currently tested. 

`GT_MOCK_OF(function)`

Macro. `gt_mockof_##function`. Uniformises the name of mock functions. 

`gt_set_mock(function, data)`

Macro. Required to register a mock and its data. 

`gt_unmocked_section(mock)`

Macro. Saves current registered mock into `mock`. Sets registered mock to NULL. Once section is executed, saved mock is re-registered. 

`GT_STRUCT_OF(function)`

Macro. `struct gt_structof##function`. Uniformises the name of the mock's data structs.

`gt_get_data(function)`

Macro. Gets the pointer to the mock's data. Parameter `function` is the name of the mocked function, used to cast void pointer to pointer of `GT_STRUCT_OF(function)`. 

`gt_assert(a, mode, b)`

Macro. Asserts that expression `a` and `b` with comparaison `mode` is true. `mode` can be one of the following values: GT_MODE_EQUALS, GT_MODE_NT_EQ (not equal), GT_MODE_LESSR, GT_MODE_BIGGR, GT_MODE_LE_EQ (lesser or equal), GT_MODE_GR_EQ (greater or equal). Do not use GT_MODE_NUMBR. The type of expression `a` is used for expression `b`. 
