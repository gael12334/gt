#
# Copyright (c) 2026 Gaël Fortier <gael.fortier.1@ens.etsmtl.ca>
#

CONFIG_KEYS=(
    "REQUIRES"
    "NAME"
    "SRC"
    "ENTRYPOINT"
    "INCLUDE"
    "LIB"
    "ARGS"
    "TYPE"
    "AUTOBACKUP"
)
declare -A CONFIGS=()

function load_configs {
    local name=$1;

    if [ ! -f "$name" ];
    then
        echo "$name not found";
        exit 1
    fi;

    for key in ${CONFIG_KEYS[@]};
    do
        local entry=$(cat $name | grep -Eo "${key}\S*=.+");
        local value=$(echo $entry | sed -E "s/.*\s*=\s*//");
        CONFIGS[$key]=$value;
        #echo "$key ${CONFIGS["$key"]}";
        #echo "$key = $value";
    done;
}

function get_last_modified_file {
    local a=$1
    local b=$2
    local a_epoch=0
    local b_epoch=0

    if [ -f $a ]
    then a_epoch=$(date -r $a "+%s")
    fi

    if [ -f $b ]
    then b_epoch=$(date -r $b "+%s")
    fi

    if [ $a_epoch -gt $b_epoch ]
    then echo 0
    else echo 1
    fi
}

function create_directory {
    local created=0
    if [ ! -d "$1" ]
    then
        mkdir "$1"
        created=1
    fi;

    if [[ $# -eq 2 && "$2" == "dir-time" && created -eq 0 ]]
    then echo $(stat -c %Y $1)
    elif [[ $# -eq 2 && "$2" == "dir-time" && created -eq 1 ]]
    then echo 0
    elif [[ $# -eq 2 && "$2" == "echo-off" ]]
    then printf ""
    else echo $1
    fi
}

function assert_directory {
    if [ ! -d "$1" ]
    then
        echo "directory $1 not found."
        exit 1
    fi
}

function auto_backup {
    local name=${CONFIGS[NAME]}
    local source_dir=${CONFIGS[SRC]}
    local entrypoint=${CONFIGS[ENTRYPOINT]}
    local include_dir=${CONFIGS[INCLUDE]}
    local lib_dir=${CONFIGS[LIB]}
    local args=${CONFIGS[ARGS]}
    local autoback=${CONFIGS[AUTOBACKUP]}
    local type=${CONFIGS[TYPE]}
    local backup_dir="backup"

    print_centered "auto backup"

    if [ "$autoback" == "disabled" ]
    then
        echo "auto-backup is disabled"
        print_centered "no backup generated"
        return
    fi

    local last_backup=$(create_directory $backup_dir dir-time)
    local current_time=$(date "+%s")
    local diff=$((3600 - ($current_time - $last_backup)))
    local path="$backup_dir/$(date '+%Y%m%d-%Hh%Mm%Ss-%s').zip"
    local exit_code=0

    if [ $diff -le 0 ]
    then
        echo $last_backup
        echo $diff
        zip "$path" $(find . -print) -x $backup_dir
        exit_code=$?
    fi

    if [[ $diff -le 0 && exit_code -eq 0 ]]
    then print_centered "saved backup"
    elif [[ $diff -le 0 && exit_code -ne 0 ]]
    then print_centered "failed to create backup"
    else print_centered "$diff seconds before next backup"
    fi
}

function print_centered {
    local text=$1
    # ----
    local text_length=${#text};
    local bar_width=43;
    local diff=$(($bar_width - $text_length))
    local start_index=$(($diff / 2));

    for i in $(seq 1 $start_index);
    do printf "=";
    done;
    printf " $text ";
    for i in $(seq $(($text_length + $start_index + 2)) $bar_width);
    do printf "=";
    done;
    printf "\n"
}

function build_cache {
    local name=${CONFIGS[NAME]}
    local source_dir=${CONFIGS[SRC]}
    local include_dir=${CONFIGS[INCLUDE]}
    local lib_dir=${CONFIGS[LIB]}
    local requires=${CONFIGS[REQUIRES]}
    local args="-ggdb -Werror -Wreturn-type -I$include_dir -L$lib_dir";
    local cache="cache"
    local failed=0
    local built=0
    local to_recompile=()
    local start=0
    local end=0

    if [ $requires != "" ]
        ./build.sh run $requires
        local exit_code=$?
        if [ $exit_code -ne 0 ]
        then exit 1
        fi
    fi

    assert_directory $source_dir
    create_directory $cache echo-off
    print_centered "caching"

    for c_file in $(find $source_dir -name "*.c" -printf "%p ")
    do
        path="$cache"
        o_file="$path/${c_file%%.c}.o"
        directories=(${c_file//"/"/" "})

        for dir in ${directories[@]:0:${#directories[@]}-1}
        do path=$(create_directory $path/$dir)
        done

        last_modified=$(get_last_modified_file $c_file $o_file)

        if [ $last_modified -eq 0 ]
        then to_recompile+=($c_file)
        else printf "%-33s up to date\n" "$c_file"
        fi
    done

    for c_file in ${to_recompile[@]}
    do
        printf "%-33s" "$c_file"
        o_file="$cache/${c_file%%.c}.o"
        start=$(date "+%s")
        gcc -o $o_file -r $c_file $args
        exit_code=$?
        end=$(date "+%s")
        delta=$(($end - $start))
        if [ $exit_code -ne 0 ]
        then
            echo " failed\n"
            failed=$(($failed + 1))
        else
            echo " compiled (${delta}s)"
            built=$(($built + 1))
        fi
    done

    print_centered "$built built, $failed failed"

    if [ $failed -gt 0 ]
    then exit 1
    fi
}

function build_program {
    local name=${CONFIGS[NAME]}
    local source_dir=${CONFIGS[SRC]}
    local entrypoint=${CONFIGS[ENTRYPOINT]}
    local include_dir=${CONFIGS[INCLUDE]}
    local lib_dir=${CONFIGS[LIB]}
    local args=${CONFIGS[ARGS]}
    local autoback=${CONFIGS[AUTOBACKUP]}
    local type=${CONFIGS[TYPE]}
    local output="output"
    local cache="cache"
    local gcc_args="$args -I$include_dir -L$lib_dir --entry=$entrypoint"
    local elf_file="$output/$name.elf"
    local modified=0
    local exit_code=0
    local o_files=()

    if [ "$type" == "lib" ]
    then return
    fi

    create_directory $output echo-off
    create_directory $cache echo-off
    print_centered "generating executable"

    o_files=$(find "$cache/$source_dir" -name "*.o" -printf "%p ")
    for o_file in ${o_files[@]}
    do
        last_modified=$(get_last_modified_file $elf_file $o_file)
        modified=$(($modified + $last_modified))
        if [ $last_modified -eq 1 ]
        then printf "%-33s modified\n" "$o_file"
        else printf "%-33s up to date\n" "$o_file"
        fi
    done

    if [ $modified -gt 0 ]
    then
        printf "%-33s" "$elf_file"
        start=$(date "+%s")
        gcc -o $elf_file ${o_files[@]} $gcc_args
        exit_code=$?
        end=$(date "+%s")
        delta=$(($end-$start))

        if [ $exit_code -ne 0 ]
        then printf " failed\n"
        else printf " compiled\n"
        fi
    fi

    if [[ $modified -gt 0 && $exit_code -eq 0 ]]
    then print_centered "executable generated"
    elif [[ $modified -gt 0 && $exit_code -ne 0 ]]
    then print_centered "generation failed"
    else print_centered "up to date"
    fi
}

function build_library {
    local name=${CONFIGS[NAME]}
    local source_dir=${CONFIGS[SRC]}
    local autoback=${CONFIGS[AUTOBACKUP]}
    local type=${CONFIGS[TYPE]}
    local output="output"
    local cache="cache"
    local lib_file="$output/lib$name.a"
    local modified=0
    local exit_code=0
    local o_files=()

    if [ "$type" == "elf" ]
    then return
    fi

    create_directory $output echo-off
    create_directory $cache echo-off
    print_centered "generating library"

    o_files=$(find "$cache/$source_dir" -name "*.o" -printf "%p ")
    for o_file in ${o_files[@]}
    do
        last_modified=$(get_last_modified_file $lib_file $o_file)
        modified=$(($modified + $last_modified))
        if [ $last_modified -eq 1 ]
        then printf "%-33s modified\n" "$o_file"
        else printf "%-33s up to date\n" "$o_file"
        fi
    done

    if [ $modified -gt 0 ]
    then
        printf "%-33s" "$lib_file"
        start=$(date "+%s")
        ar rcs $lib_file ${o_files[@]}
        exit_code=$?
        end=$(date "+%s")
        delta=$(($end-$start))

        if [ $exit_code -ne 0 ]
        then printf " failed\n"
        else printf " compiled\n"
        fi
    fi

    if [[ $modified -gt 0 && $exit_code -eq 0 ]]
    then print_centered "library generated"
    elif [[ $modified -gt 0 && $exit_code -ne 0 ]]
    then print_centered "generation failed"
    else print_centered "up to date"
    fi
}

function build_backup {
    local backup_dir="backup"
    local path="$backup_dir/$(date '+%Y%m%d-%Hh%Mm%Ss-%s').zip"

    print_centered "generating backup"
    create_directory $backup_dir echo-off
    zip "$path" $(find . -print) -x $backup_dir

    if [ $? -eq 0 ];
    then print_centered "saved backup"
    else print_centered "failed to create backup"
    fi
}

function main {
    local argc=$1;
    local argv=(${*:2:$(($#-1))});

    if [[ $argc -ge 2 && "${argv[0]}" == "cache" ]];
    then
        load_configs "./${argv[1]}.cfg"
        build_cache
    elif [[ $argc -ge 2 && "${argv[0]}" == "run" ]];
    then
        load_configs "./${argv[1]}.cfg"
        build_cache
        build_program
        build_library
        auto_backup
    elif [[ $argc -ge 1 && "${argv[0]}" == "backup" ]];
    then
        build_backup $argc ${argv[@]}
    else
        echo "options: cache, run, backup"
    fi
}

main $# $@
