#
# Copyright © 2025 Gaël Fortier <gael.fortier.1@ens.etsmtl.ca>
#
if [ ! -d "./out" ]; then
    mkdir ./out
fi
gcc -o ./out/program -Werror -Wreturn-type -ggdb $(find . -name '*.c' -print) -I/usr/include -L/usr/lib -static-libgcc
chmod +x ./out/program
