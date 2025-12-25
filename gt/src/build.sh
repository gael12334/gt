#
# Copyright © 2025 Gaël Fortier <gael.fortier.1@ens.etsmtl.ca>
#

if [ ! -d "../out" ]; then
    mkdir ../out
fi

echo "gt ........... compiling"

version=$(cat version.mv)
gcc -o gt.o -c gt.c -ggdb -static -static-libgcc -Werror -Wreturn-type -I/usr/include -L/usr/lib
if [ $? -ne 0 ];
then
    exit
fi
cp gt.h "../out/gt${version}.h"
mv gt.o "../out/gt${version}.o"
cp version.mv "../out/gt${version}.mv"

echo "gt ........... done"
