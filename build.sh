#
# Copyright © 2025 Gaël Fortier <gael.fortier.1@ens.etsmtl.ca>
#

if [ ! -d "./.out" ]; then
    mkdir ./.out
fi
gcc -o ./.out/program -Werror -Wreturn-type -ggdb ./src/gt.c ./src/main.c ./src/patcher.c -I/usr/include -L/usr/lib -static -static-libgcc
chmod +x ./.out/program
