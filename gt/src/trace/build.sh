#!/bin/sh
#
# Copyright (c) 2026 Gaël Fortier <gael.fortier.1@ens.etsmtl.ca>
#

if [ ! -d out ]; then
    mkdir out
fi

gcc -o out/trace.o trace.c -c -ggdb -L/usr/lib -I/usr/include
if [ $? -ne 0 ]; then
    exit 1
fi
