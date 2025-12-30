#
# Copyright (c) 2025 Gaël Fortier <gael.fortier.1@ens.etsmtl.ca>
#

source "./build/assert.sh"
source "./build/dir.sh"

function get_filename() {
    local path=$1
    readarray -d "/" -t arr <<< $path
    assert $?
    local len=${#arr[*]}
    echo ${arr[len-1]}
}

function copy_artifacts() {
    local prj=$1
    local dir=$2
    local ext="*.$3"
    local name=""

    readarray -d "\0" -t files <<< $(find $prj/out -name $ext -print)
    assert $?

    for i in $files;
    do
        name=$(get_filename $i)
        cp $i "pkg/$dir/$name"
        assert $?
    done
    unset i
    unset files
}

function write_version_list() {
    local prj=$1
    local ext="*.mv"
    readarray -d "\0" -t files <<< $(find $prj/src -name $ext -print)
    assert $?

    for i in $files;
    do
        version=$(cat $i)
        echo $prj $version >> pkg/list.mv
        assert $?
    done
    unset i
    unset files
}

function build_projects() {
    local proj=$1
    cd $proj
    assert $?
    cd src
    assert $?
    ./build.sh
    assert $?
    cd ..
    cd ..
}

if [ "$1" = "clean" ];
then
    projects=$(cat projects)
    assert $?

    deldir pkg
    assert $?

    for p in $projects;
    do
        #rm -rf "$p/out"
        deldir $p/out
        assert $?
    done
    unset p
fi

newdir pkg
newdir pkg/lib
newdir pkg/bin
newdir pkg/include

projects=$(cat projects)
assert $?

for p in $projects;
do
    build_projects "$p"
done

rm pkg/list.mv
assert $?

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
