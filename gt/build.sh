#
# Copyright (c) 2025 Gaël Fortier <gael.fortier.1@ens.etsmtl.ca>
#

function assert() {
    if [ $1 -ne 0 ];
    then exit;
    fi
}

function create_path() {
    local path=$1
    readarray -d "/" -t arr <<< $path
    assert $?

    i=0
    p=""
    len=${#arr[@]}
    while [ $i -lt $len ]; do
        p="$p/${arr[$i]}"
        i=$(expr $i + 1)
        assert $?
        if [ $i -ne $len ]; then
            dir=".$p"
            if [ ! -d $dir ]; then
                mkdir $dir
                assert $?
            fi
        fi
    done
}

function precompile() {
    local name=$1
    local source=$2
    local object=$3
    local OBJECTS_GCC_ARGS=$4

    if [ -f $object ]; then
        src_last_changed=$(date -r $source "+%s")
        obj_last_changed=$(date -r $object "+%s")

        if [ $src_last_changed -lt $obj_last_changed ]; then
            printf "[$name]: $source \n"
            printf "[$name]: :... Up to date \n"
            return
        fi
    fi

    compile "$name" "$object" "$source" "-r" "$OBJECTS_GCC_ARGS"
}

function compile() {
    # params
    local name=$1
    local output_name=$2
    local c_files=$3
    local linker_args=$4
    local OBJECTS_GCC_ARGS=$5

    # command
    local command="gcc -o $output_name $c_files $linker_args $OBJECTS_GCC_ARGS"
    printf "[$name]: $command\n"

    # execution
    $($command)
    if [ $? -eq 0 ]; then
        printf "[$name]: :... Succeeded\n"
    else
        printf "[$name]: :... Failed\n"
        exit 1
    fi
}

# ---------------------
# project name
PROJECT_NAME="gt"
# output directory
OUT_DIRECTORY="out"
# precompiled object files
OBJECT_FILES=$()
# source code directory
SOURCE_DIRECTORY="src"
# source code files
SOURCE_FILES=$(find "$SOURCE_DIRECTORY" -name "*.c" -printf "%p ")

# ---------------------
create_path $OUT_DIRECTORY

# ---------------------
# GCC arguments for precompiled object files
OBJECTS_GCC_ARGS="-ggdb -static -static-libgcc -Werror -Wreturn-type -I/usr/include -L/usr/lib"
# precompiles every .c file into a .o file
for file in $SOURCE_FILES; do
    objfile=${file##$SOURCE_DIRECTORY}
    objfile=${objfile%%".c"}
    objfile="$OUT_DIRECTORY$objfile.o"
    OBJECT_FILES+=($objfile)
    precompile "$PROJECT_NAME" "$objfile" "$file" "$OBJECTS_GCC_ARGS"
done

# ---------------------
# WORK IN PROGRESS
#
#
