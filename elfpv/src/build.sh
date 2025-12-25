#
# Copyright © 2025 Gaël Fortier <gael.fortier.1@ens.etsmtl.ca>
#

if [ ! -d "../out" ]; then
    mkdir ../out
fi

echo "elfpv ........ compiling"

version=$(cat version.mv)
gcc -o elfpv.elf elfpv.c elfpv_main.c -ggdb -static -static-libgcc -Werror -Wreturn-type -I/usr/include -L/usr/lib
if [ $? -ne 0 ];
then
    exit
fi

gcc -o elfpv.o -c elfpv.c -ggdb -static -static-libgcc -Werror -Wreturn-type -I/usr/include -L/usr/lib
if [ $? -ne 0 ];
then
    exit
fi

mv elfpv.elf "../out/elfpv${version}.elf"
mv elfpv.o "../out/elfpv${version}.o"
cp elfpv.h "../out/elfpv${version}.h"
cp version.mv "../out/elfpv${version}.mv"
chmod +x "../out/elfpv${version}.elf"

echo "elfpv ........ done"
