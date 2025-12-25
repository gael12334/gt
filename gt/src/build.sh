#
# Copyright © 2025 Gaël Fortier <gael.fortier.1@ens.etsmtl.ca>
#

function assert_success() {
    if [ $1 -ne 0 ];
    then
        exit
    fi
}

if [ ! -d "../out" ]; then
    mkdir ../out
fi

echo "gt ........... compiling"

gcc -o gt.o -c gt.c -ggdb -static -static-libgcc -Werror -Wreturn-type -I/usr/include -L/usr/lib
assert_success $?

version=$(cat version.mv)
assert_success $?

cp gt.h "../out/gt${version}.h"
assert_success $?

mv gt.o "../out/gt${version}.o"
assert_success $?

cp version.mv "../out/gt${version}.mv"
assert_success $?

echo "gt ........... done"
