#
# Copyright (c) 2026 Gaël Fortier <gael.fortier.1@ens.etsmtl.ca>
#

output_cli="cli.elf"
output_gt="gt.o"
output_demo="demo.elf"
output_lib="libgt.a"

function build_cli {
    local outputfile=$output_cli
    local srcfiles="$(find src -name "*.c" -printf '%p ')"
    local command="gcc -o $outputfile $srcfiles -ggdb -Werror -Wreturn-type"
    echo $command
    $command
    if [ $? -eq 0 ]
    then
        chmod +x $outputfile
        echo "  Output: $outputfile"
        return 0
    fi
    return 1
}

function build_gt {
    local outputfile=$output_gt
    local srcfiles="$(find src -name "*.c" -printf '%p ')"
    local command="gcc -o $outputfile -r ${srcfiles/'src/cli.c'/''} -ggdb -Werror -Wreturn-type -fpatchable-function-entry=50"
    echo $command
    $command
    if [ $? -eq 0 ]
    then
        chmod +x $outputfile
        echo "  Output: $outputfile"
        return 0
    fi
    return 1
}

function build_demo {
    if [ ! -f "$output_gt" ]
    then build_gt
    fi
    local outputfile=$output_demo
    local demofiles=" $(find demo -name "*.c" -printf '%p ') $output_gt"
    local command="gcc -o $outputfile $demofiles -ggdb -Werror -Wreturn-type -fpatchable-function-entry=50 -lm -lc -fno-stack-protector"
    echo $command
    $command
    if [ $? -eq 0 ]
    then
        chmod +x $outputfile
        echo "  Output: $outputfile"
        return 0
    fi
    return 1
}

function build_lib {
    local outputfile=$output_lib
    local command="ar rcs $outputfile $output_gt"
    echo $command
    $command
    if [ $? -eq 0 ]
    then
        chmod +x $outputfile
        echo "  Output: $outputfile"
        return 0
    fi
    return 1
}

function build_all {
    build_cli
    build_gt
    build_demo
    build_lib
}

function build_test {
    if [ ! -f "cli.elf" ]
    then build_cli
    fi
    build_gt
    build_demo
    ./cli.elf elf demo.elf patch main
    ./out
}

declare -A artefacts=(
    ["cli"]="build_cli"
    ["gt"]="build_gt"
    ["demo"]="build_demo"
    ["lib"]="build_lib"
    ["all"]="build_all"
    ["test"]="build_test"
)

if [[ $# -ne 1 ]]; then
    echo "Usage: $0 [${!artefacts[@]}]"
    exit 1
fi

current_time=$(date "+%s")
outputfile=""
${artefacts[$1]}
exitcode=$?

if [ $exitcode -ne 0 ]; then
    echo "Usage: $0 [${!artefacts[@]}]"
    exit 1
fi

backup_dir="backup"
last_backup=0

if [ ! -d "$backup_dir" ];
then mkdir "$backup_dir"
else last_backup=$(stat -c %Y $backup_dir); fi

exitcode=0
diff=$((3600 - ($current_time - $last_backup)))
path="$backup_dir/$(date '+%Y%m%d-%Hh%Mm%Ss-%s').zip"

if [ $diff -le 0 ]; then
    out=$(zip "$path" $(find . -print) -x "${backup_dir}/*")
    exitcode=$?
    if [ $exitcode -eq 0 ];
    then echo "  saved backup"
    else echo "  failed to create backup"; fi
else  echo "  $diff seconds before next backup"; fi
