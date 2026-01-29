#!/bin/bash
#
# Copyright (c) 2026 Gaël Fortier <gael.fortier.1@ens.etsmtl.ca>
#

function build_dependency {
    local loc=$1
    local script=$2
    local out=$3
    local dir=$(pwd)

    cd $loc
    if [ $? -ne 0 ]; then
        exit 1
    fi

    ./$script
    if [ $? -ne 0 ]; then
        exit 1
    fi

    cp out/$out $dir/out/$out
    if [ $? -ne 0 ]; then
        exit 1
    fi

    cd $dir
    return 0
}

if [ ! -d out ]; then
    mkdir out
fi

build_dependency ../trace build.sh trace.o
if [ $? -ne 0 ]; then
    exit 1
fi

gcc -o out/buf.o buf.c -c -ggdb -L/usr/lib -I/usr/include
if [ $? -ne 0 ]; then
    exit 1
fi
