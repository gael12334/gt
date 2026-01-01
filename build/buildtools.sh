#
# Copyright (c) 2025 Gaël Fortier <gael.fortier.1@ens.etsmtl.ca>
#

declare FALSE=0
declare TRUE=1

# ---------------------------
# Utilities
# ---------------------------

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

function last_mod_timestamp_file() {
    local output_directory=$1
    local selected_file=$2
    echo "${output_directory}/${selected_file}.txt"
}

function file_changed() {
    # parameters
    local last_mod_file=$1
    local selected_file=$2

    # if last modification is unsaved,
    # then assumes there was a change.
    if [ ! -f $last_mod_file ];
    then
        echo $TRUE
        return
    fi

    # reads saved timestamp
    local saved=$(cat $last_mod_file)
    assert_ret $?

    # reads current source modification timestamp
    local source=$(date -r $selected_file "+%s")
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

function named_list_str() {
    local name=$1
    local list=$2

    readarray -d " " -t arr <<< $list
    assert_ret $?

    local len=${#arr[@]}
    echo "$name $len ${arr[*]}"
}

# ---------------------------
# Build functions
# ---------------------------

function build_get_output_filepath() {
    local dst_path=$1
    local src_file=$2
    echo "${dst_path}/${src_file}.o"
}

function build_make_elfobj() {
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

RESULT_build_check_changes=0
function build_check_changes() {
    # parameters
    local dst_path=$1
    local src_file=$2

    # object file path
    local dst_file=$(build_get_output_filepath $dst_path $src_file)

    # last modification file path
    local last_mod=$(last_mod_timestamp_file $dst_path $src_file)

    # asserts that source file exists
    assert_file $src_file

    # compiles source to elf object if
    # source files have changed.
    changed=$(file_changed $last_mod $src_file)
    if [ $changed -eq $TRUE ];
    then
        build_make_elfobj $src_file $dst_file
        echo $(date -r $src_file "+%s") > $last_mod
        assert_ret $?
        echo "[ Compiled ] $src_file"
        echo "             :... $dst_file"

        RESULT_build_check_changes+=1
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
    local lnk_argumnt=$5

    RESULT_build_check_changes=0
    objfiles_arr=()

    # compiles every source file into object files
    for file in $source_file
    do
        echo $file
        build_check_changes $output_path $file
        objfiles_arr+=($(build_get_output_filepath $output_path $file))
    done

    # if result = 0 and output file exists,
    # project already up to date.
    if [ $RESULT_build_check_changes -eq 0 ];
    then
        if [ -f $output_file ];
        then
            echo "[Up to date] $output_file"
            echo "             :... No output generated."
            return
        fi
    fi

    # clean up
    RESULT_build_check_changes=0

    # compiles the output file
    gcc -o $output_file ${objfiles_arr[*]} $gcc_argumnt
    assert_ret $?

    #
    echo "[ Compiled ] gcc -o $output_file ${objfiles_arr[*]} $lnk_argumnt $gcc_argumnt"
    echo "             :... $output_file"
}

# ---------------------------
# Publish functions
# ---------------------------

function publish_destpath() {
    local dir=$1
    local name=$2
    local vers=$3

    echo "$dir/${name}_${vers}"
}

RESULT_publish_check_changes=0
function publish_check_changes() {
    # parameters
    local dst_path=$1
    local src_file=$2

    # object file path
    local dst_file=$(build_get_output_filepath $dst_path $src_file)

    # last modification file path
    local last_mod=$(last_mod_timestamp_file $dst_path $src_file)

    # asserts that source file exists
    assert_file $src_file

    # compiles source to elf object if
    # source files have changed.
    changed=$(file_changed $last_mod $src_file)

    if [ $changed -eq $TRUE ];
    then
        echo "[ Publised ] $src_file"
        echo "             :... $dst_file"
    else
        echo "[Up to date] $src_file"
        echo "             :... File not copied."
    fi
}

function publish() {
    # parameters
    local proj_name=$1
    local proj_vers=$2
    local pblsh_dir=$3
    local hdr_files=$4
    local lib_files=$5
    local bin_files=$6
    local headr_dir=$7
    local libry_dir=$8
    local binry_dir=$9

    RESULT_publish_check_changes=0
    dest_path=$(publish_destpath $pblsh_dir $proj_name $proj_vers)
    echo $dest_path

    for hdr in $hdr_files
    do
        publish_check_changes $dest_path $hdr
    done

    for lib in $lib_files
    do
        publish_check_changes $dest_path $lib
    done

    for bin in $bin_files
    do
        publish_check_changes $dest_path $bin
    done

    # if result = 0 and output file exists,
    # project already up to date.
    if [ $RESULT_publish_check_changes -eq 0 ];
    then
        echo "[Up to date] $output_file"
        echo "             :... No output generated."
        return
    fi

    # clean up
    RESULT_build_check_changes=0

    echo "[ Publised ] $proj_name"
    echo "             :... $dest_path"
}
