#! /bin/sh

name=$1

if [ $# -eq 0 ]; then
    echo need a file to test.
    exit 1
fi

if [ ! -f $name ]; then
    echo $name was moved, renamed or deleted.
    exit 1
fi

if [ ! -d ./tmp ]; then
    mkdir tmp
fi

gcc -o ./tmp/${name}.out ./${name} ./errors.c
if [ $? -ne 0 ]; then
    echo compilation failed, aborting.
    exit 1
fi

chmod +x ./tmp/${name}.out
./tmp/${name}.out
