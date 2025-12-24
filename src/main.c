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
 *
 *
 * 48 8d 1d 80 2e 00 00 4c 8d 25 ea ff ff ff 49 3b dc 0f 84 1b 00 00 00 48 8d 1d 71 2e 00 00 48 c7 c0 00 00 00 00 ff d3
 *
 * 48 8d 1d 71 2e 00 00 48 c7 c0 00 00 00 00 ff d3
 *
 * 90 48 8b 05 83 2e 00 00 4c 8b 25 ed ff ff ff 49 3b c4 0f 84 1a 00 00 00 55 48 89 e5 48 8b 1d 70 2e 00 00 48 c7 c0 00 00 00 00 ff d3 5d c3
 *
 * f3 0f 1e fa || 90 48 8b 05 83 2e 00 00 4c 8b 25 ed ff ff ff 49 3b c4 0f 84 1a 00 00 00 55 48 89 e5 48 8b 1d 70 2e 00 00 48 c7 c0 00 00 00 00 ff d3 5d c3
 */

#include "patcher.h"
#include <stdio.h>

int main(int argc, char** argv) {
    if(argc < 2) {
        printf("Path of ELF file required.\n");
        return 0;
    }

    if(argc > 2) {
        elf_cli_menu(argv[1], argv[2]);
        return 0;
    }

    elf_menu(argv[1]);
    return 0;
}
