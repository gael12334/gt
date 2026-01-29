#!/bin/sh
#
# Copyright (c) 2026 Gaël Fortier <gael.fortier.1@ens.etsmtl.ca>
#

./build.sh
if [ $? -ne 0 ]; then
    exit 1
fi

gcc -o out/test.bin test.c $(find out -name "*.o" -print) -ggdb -L/usr/lib -I/usr/include
if [ $? -ne 0 ]; then
    exit 1
fi

chmod +x out/test.bin
./out/test.bin
