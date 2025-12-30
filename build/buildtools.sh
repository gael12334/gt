#
# Copyright (c) 2025 Gaël Fortier <gael.fortier.1@ens.etsmtl.ca>
#

declare FALSE=0
declare TRUE=1

function assert_ret() {
    if [ $1 -ne 0 ];
    then
        echo "Assertion.";
        exit;
    fi
}

function assert_args() {
    local expected=$1
    local recieved=$2
    if [ $expected -ne $recieved ]
    then
        echo "Expected $expected args, got $recieved."
        exit;
    fi
}

function assert_file() {
    if [[ ! -f $0 ]]
    then
        echo "$0 not found."
        exit;
    fi
}

function make_dir() {
    local name=$1
    if [ ! -d $name ];
    then
        mkdir $name;
        assert_ret $?;
    fi
}

function del_dir() {
    local name=$1
    if [ ! -d $name ];
    then
        echo "[error] Directory ${name} does not exist."
        exit
    fi
    rm -rf $name
    assert $?
}

function make_path() {
    local path=$1
    readarray -d "/" -t arr <<< $path
    assert_ret $?

    i=0
    p=""
    len=$(expr ${#arr[@]})

    while [ $i -lt $len ]
    do
        p="$p/${arr[$i]}"
        i=$(expr $i + 1)
        if [ $i -ne $len ];
        then make_dir ".$p"
        fi
    done
}

function getsrc_objname() {
    local dst_path=$1
    local src_file=$2
    echo "${dst_path}/${src_file}.o"
}

function getsrc_lastmodname() {
    local dst_path=$1
    local src_file=$2
    echo "${dst_path}/${src_file}.txt"
}

function make_elfobj() {
    # parameters
    local src_file=$1
    local dst_file=$2

    # makes sure the directories exists before
    # creating elf object files.
    make_path $dst_file

    # executes GCC with '-r' flag to create a
    # relocatable object file.
    gcc -o $dst_file $src_file -r -ggdb -O1

    # checks if GCC succeeded
    assert_ret $?
}

function src_changed() {
    # parameters
    local savedlstmod=$1
    local source_file=$2

    # if last modification is unsaved,
    # then assumes source code changed.
    if [ ! -f $savedlstmod ];
    then
        echo $TRUE
        return
    fi

    # reads saved timestamp from previous build
    local saved=$(cat $savedlstmod)
    assert_ret $?

    # reads current source modification timestamp
    local source=$(date -r $source_file "+%s")
    assert_ret $?

    # if timestamp saved equals source,
    # then source code unchanged.
    if [ $source -eq $saved ];
    then
        echo $FALSE
        return
    fi

    echo $TRUE
}

RESULT_check_changes=0
function check_changes() {
    # parameters
    local dst_path=$1
    local src_file=$2

    # object file path and file of
    # last modification of source file
    local dst_file=$(getsrc_objname $dst_path $src_file)
    local last_mod=$(getsrc_lastmodname $dst_path $src_file)

    # asserts that source file exists
    assert_file $src_file

    # compiles source to elf object if
    # source files have changed.
    changed=$(src_changed $last_mod $src_file)
    if [ $changed -eq $TRUE ];
    then
        make_elfobj $src_file $dst_file
        echo $(date -r $src_file "+%s") > $last_mod
        echo "[ Compiled ] $src_file"
        echo "             :... $dst_file"
        RESULT_check_changes+=1
    else
        echo "[Up to date] $src_file"
        echo "             :... No object file generated."
    fi
}

function build() {
    local output_path=$1
    local output_file=$2
    local source_path=$3
    local source_file=$4
    local gcc_argumnt=$5

    RESULT_check_changes=0
    objfiles_arr=()

    # compiles every source file into object files
    for file in $source_file
    do
        check_changes $output_path $file
        objfiles_arr+=($(getsrc_objname $output_path $file))
    done

    # if result = 0 and output file exists,
    # project already up to date.
    if [ $RESULT_check_changes -eq 0 ];
    then
        if [ -f $output_file ];
        then
            echo "[Up to date] $output_file"
            echo "             :... No output generated."
            return
        fi
    fi

    # clean up
    RESULT_check_changes=0

    # compiles the output file
    gcc -o $output_file ${objfiles_arr[*]} $gcc_argumnt
    assert_ret $?

    #
    echo "[ Compiled ] gcc -o $output_file ${objfiles_arr[*]} $gcc_argumnt"
    echo "             :... $output_file"
    return
}
