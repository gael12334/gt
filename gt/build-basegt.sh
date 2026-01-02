#
# Copyright (c) 2026 Gaël Fortier <gael.fortier.1@ens.etsmtl.ca>
#
if [ ! -d out ]; then
    mkdir out
fi
gcc -o out/gt.elf out/elf/lib.o src/gt.c -ggdb -static -static-libgcc -Werror -Wreturn-type -I/usr/include -L/usr/lib
chmod +x out/gt.elf
