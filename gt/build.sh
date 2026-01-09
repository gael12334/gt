#
# Copyright (c) 2026 Gaël Fortier <gael.fortier.1@ens.etsmtl.ca>
#

declare -a srcfiles="$(find src -name "*.c" -print)"
declare -a demofiles="$(find demo -name "*.c" -print)"
declare -A commands=(
    ["cli"]="gcc -o cli.elf $srcfiles -ggdb -Werror -Wreturn-type"
    ["gt"]="gcc -o gt.o -r ${srcfiles/'src/elf/cli.c'/''} -ggdb -Werror -Wreturn-type -fpatchable-function-entry=50"
    ["demo"]="gcc -o demo.elf $demofiles -ggdb -Werror -Wreturn-type -fpatchable-function-entry=50"
    ["lib"]="ar rcs libgt.a gt.o"
)

current_time=$(date "+%s")
outputfile=""
exitcode=0
if [[ $# -eq 1 && "$1" == "cli" ]]; then
    outputfile="cli.elf"
    echo ${commands[$1]}
    ${commands[$1]}
elif [[ $# -eq 1 && "$1" == "gt" ]]; then
    outputfile="gt.o"
    echo ${commands[$1]}
    ${commands[$1]}
elif [[ $# -eq 1 && "$1" == "demo" ]]; then
    outputfile="demo.elf"
    echo ${commands[$1]}
    ${commands[$1]}
elif [[ $# -eq 1 && "$1" == "lib" ]]; then
    outputfile="libgt.a"
    echo ${commands[$1]}
    ${commands[$1]}
fi

if [ $exitcode -eq 0 ]; then
    chmod +x $outputfile
    echo "  Output: $outputfile"
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
