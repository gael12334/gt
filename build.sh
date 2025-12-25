#
# Copyright © 2025 Gaël Fortier <gael.fortier.1@ens.etsmtl.ca>
#

function assert_success() {
    if [ $1 -ne 0 ];
    then
        exit
    fi
}

function new_dir() {
    local name=$1
    if [ ! -d $name ];
    then
        mkdir $name
    fi
}

function get_filename() {
    local path=$1
    readarray -d "/" -t arr <<< $path
    assert_success $?
    local len=${#arr[*]}
    echo ${arr[len-1]}
}

function copy_artifacts() {
    local prj=$1
    local dir=$2
    local ext="*.$3"
    local name=""

    readarray -d "\0" -t files <<< $(find $prj/out -name $ext -print)
    assert_success $?

    for i in $files;
    do
        name=$(get_filename $i)
        cp $i "pkg/$dir/$name"
        assert_success $?
    done
    unset i
    unset files
}

function write_version_list() {
    local prj=$1
    local ext="*.mv"
    readarray -d "\0" -t files <<< $(find $prj/src -name $ext -print)
    assert_success $?

    for i in $files;
    do
        version=$(cat $i)
        echo "$prj $version" >> pkg/list.mv
        assert_success $?
    done
    unset i
    unset files
}

function build_projects() {
    local proj=$1
    cd $proj
    assert_success $?
    cd src
    assert_success $?
    ./build.sh
    assert_success $?
    cd ..
    cd ..
}

if [ "$1" = "clean" ];
then
    projects=$(cat projects)
    assert_success $?

    rm -rf pkg
    assert_success $?

    for p in $projects;
    do
        rm -rf "$p/out"
        assert_success $?
    done
    unset p
fi

new_dir pkg
new_dir pkg/lib
new_dir pkg/bin
new_dir pkg/include

projects=$(cat projects)
assert_success $?

for p in $projects;
do
    build_projects "$p"
done

> pkg/list.mv
assert_success $?

for p in $projects;
do
    if [ "$p" = "" ];
    then
        continue
    fi
    copy_artifacts $p "lib" "o"
    copy_artifacts $p "include" "h"
    copy_artifacts $p "bin" "elf"
    write_version_list $p
done
