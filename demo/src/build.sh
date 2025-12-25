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

echo "demo ......... compiling"

filename="demo"
version=$(cat version.mv)
assert_success $?

if [ "$1" = "test" ];
then
    gt_obj="../../pkg/lib/gt1.o"
    out_obj="../out/patchable_${filename}${version}.o"
    test_src="../test/demo_test.c"

    gcc -o $out_obj $gt_obj $test_src demo.c -O0 -ggdb -static -static-libgcc -fpatchable-function-entry=50 -fpcc-struct-return -Werror -Wreturn-type -I/usr/include -L/usr/lib
    assert_success $?

    ../../pkg/bin/elfpv1.elf $out_obj _gt_hjck _gt_hook p
    assert_success $?

    mv out $out_obj
    chmod +x $out_obj

    #out_test_elf="../out/test_${filename}${version}.elf"
    #
    #gcc -o out_test_elf $out_obj $test_src -ggdb -static -static-libgcc -Werror -Wreturn-type -I/usr/include -L/usr/lib
    #assert_success $?
else
    out_obj="../out/${filename}${version}.o"
    gcc -o $out_obj -c demo.c -ggdb -static -static-libgcc -Werror -Wreturn-type -I/usr/include -L/usr/lib
    assert_success $?
    chmod +x $out_obj
fi

echo "demo ......... done"
