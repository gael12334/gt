#
# Copyright (c) 2025 Gaël Fortier <gael.fortier.1@ens.etsmtl.ca>
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

echo "elfpv ........ compiling"

gcc -o elfpv.elf $(find . -name "*.c" -print) -ggdb -static -static-libgcc -Werror -Wreturn-type -I/usr/include -L/usr/lib
assert_success $?

gcc -o elfpv.o -r $(find . -name "*_*.c" -print) -ggdb -static -static-libgcc -Werror -Wreturn-type -I/usr/include -L/usr/lib
assert_success $?

version=$(cat version.mv)
assert_success $?

mv elfpv.elf "../out/elfpv${version}.elf"
assert_success $?

mv elfpv.o "../out/elfpv${version}.o"
assert_success $?

cp elfpv_lib.h "../out/elfpv_lib${version}.h"
assert_success $?

cp elfpv_utils.h "../out/elfpv_utils${version}.h"
assert_success $?

cp version.mv "../out/elfpv${version}.mv"
assert_success $?

chmod +x "../out/elfpv${version}.elf"
assert_success $?

echo "elfpv ........ done"
