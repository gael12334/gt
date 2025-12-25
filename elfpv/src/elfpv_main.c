/*
    Copyright © 2025 Gaël Fortier <gael.fortier.1@ens.etsmtl.ca>
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
 */

#include "elfpv.h"
#include <stdio.h>

int main(int argc, char** argv) {
    if(argc < 4) {
        puts("Usage: elfpv <elf> <hijack> <hook> [cli opts]");
        return 0;
    }

    printf("%s %s %s\n", argv[1], argv[2], argv[3]);

    elf_set_symbol_name(argv[2], argv[3]);
    (argc > 4)
        ? elf_cli_menu(argv[1], argv[4])
        : elf_menu(argv[1]);
    return 0;
}
