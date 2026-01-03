#
# Copyright (c) 2026 Gaël Fortier <gael.fortier.1@ens.etsmtl.ca>
#

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
    local argc=$((argc - 1));
    local argv=(${*:3:$(($#-1))});
    local cache="cache";
    local cfg_file="${argv[0]}.cfg";

    if [ ! -f "$cfg_file" ];
    then
        echo "$cfg_file not found";
        exit 1
    fi;

    local directory_name=$(cat $cfg_file | grep -Eo "SRC\S*=.+")
    local include_dir=$(cat $cfg_file | grep -Eo "INCLUDE\S*=.+")
    local lib_dir=$(cat $cfg_file | grep -Eo "LIB\S*=.+")
    directory_name=$(echo $directory_name | sed -E "s/.*\s*=\s*//")
    include_dir=$(echo $include_dir | sed -E "s/.*\s*=\s*//")
    lib_dir=$(echo $lib_dir | sed -E "s/.*\s*=\s*//")

    if [ ! -d "$directory_name" ];
    then
        echo "$directory_name not found";
        exit 1;
    fi;

    if [ ! -d "$cache" ];
    then mkdir "$cache";
    fi;

    echo $(print_centered "caching")
    local compiled=0;
    local unchanged=0;
    local failed=0;
    local c_files=($(find $directory_name -name "*.c" -printf "%p "));
    for c_file in ${c_files[@]};
    do
        printf "%-33s found\n" $c_file
        path="$cache";
        o_file="$cache/${c_file%%.c}.o";
        directories=(${c_file//"/"/" "});

        for dir in ${directories[@]:0:${#directories[@]}-1};
        do
            path="$path/$dir";
            if [ ! -d "$path" ];
            then mkdir "$path";
            fi;
        done;

        c_file_last_edit=$(date -r $c_file "+%s");
        o_file_last_edit=0;

        if [ -f "$o_file" ];
        then o_file_last_edit=$(date -r $o_file "+%s");
        fi

        if [ $o_file_last_edit -gt $c_file_last_edit ];
        then
            unchanged=$(($unchanged+1));
            printf "%-33s unmodified\n" " ";
            printf "%-33s ignored\n\n" " ";
            continue;
        fi

        args="-ggdb -Werror -Wreturn-type -I$include_dir -L$lib_dir";
        compiled=$(($compiled+1));
        printf "%-33s compiling\n" $o_file;
        gcc -o $o_file -r $c_file $args;
        gcc_result=$?

        if [ $gcc_result -ne 0 ];
        then printf "%-33s FAILED\n\n" " ";
        else printf "%-33s done\n\n" " ";
        fi;

        if [ $gcc_result -ne 0 ];
        then failed=$(($failed + 1));
        fi;
    done;

    local result_text="$compiled cached, $unchanged ignored";
    echo $(print_centered "$result_text");
}

function build_program() {
    local argc=$((argc - 1));
    local argv=(${*:3:$(($#-1))});
    local cfg_file="${argv[0]}.cfg";

    if [ ! -f "$cfg_file" ];
    then
        echo "$cfg_file not found";
        exit 1
    fi;

    echo $(print_centered "building");
    local name=$(cat $cfg_file | grep -Eo "NAME\S*=.+");
    local directory_name=$(cat $cfg_file | grep -Eo "SRC\S*=.+");
    local include_dir=$(cat $cfg_file | grep -Eo "INCLUDE\S*=.+");
    local lib_dir=$(cat $cfg_file | grep -Eo "LIB\S*=.+");
    local gcc_args=$(cat $cfg_file | grep -Eo "ARGS\S*=.+");
    local autoback=$(cat $cfg_file | grep -Eo "AUTOBACKUP\S*=.+");
    name=$(echo $name | sed -E "s/.*\s*=\s*//");
    directory_name=$(echo $directory_name | sed -E "s/.*\s*=\s*//");
    include_dir=$(echo $include_dir | sed -E "s/.*\s*=\s*//");
    lib_dir=$(echo $lib_dir | sed -E "s/.*\s*=\s*//");
    gcc_args=$(echo $gcc_args | sed -E "s/.*\s*=\s*//");
    autoback=$(echo $autoback | sed -E "s/.*\s*=\s*//");

    local output="output";
    local cache="cache";
    local backup="backup";

    if [ ! -d "$output" ];
    then mkdir "$output";
    fi;

    local recompile=1;
    local o_files=$(find "$cache/$directory_name" -name "*.o" -printf "%p ");
    local elf_file="$output/$name.elf";

    if [ -f $elf_file ];
    then
        recompile=0
        printf "%-33s found\n\n" "$elf_file";
        for o_file in ${o_files[@]};
        do
            printf "%-33s found\n" "$o_file";
            o_file_last_edit=$(date -r $o_file "+%s");
            elf_file_last_edit=0;

            if [ -f "$elf_file" ];
            then elf_file_last_edit=$(date -r $elf_file "+%s");
            fi

            if [ $elf_file_last_edit -lt $o_file_last_edit ];
            then printf "%-33s changed\n\n" " ";
            else printf "%-33s unmodified\n\n" " ";
            fi

            if [ $elf_file_last_edit -lt $o_file_last_edit ];
            then recompile=1;
            fi
        done;
    fi;

    if [ $recompile -eq 1 ];
    then
        printf "%-33s compiling\n" $elf_file;
        gcc -o $elf_file ${o_files[@]} $gcc_args;
        if [ $? -ne 0 ];
        then printf "%-33s FAILED\n\n" " ";
        else printf "%-33s done\n\n" " ";
        fi;
    else printf "%-33s ignored\n\n" "$elf_file";
    fi;

    echo $(print_centered "Completed");
    echo $(print_centered "Auto backup");

    if [ "$autoback" == "enabled" ];
    then
        dir_last_edit=0;
        if [ ! -d "$backup" ];
        then mkdir $backup;
        else dir_last_edit=$(stat -c %Y $backup);
        fi

        current_time=$(date "+%s");
        diff=$((3600 - ($current_time - $dir_last_edit)));
        archive_name="$backup/$(date '+%Y%m%d-%Hh%Mm%Ss-%s').zip";

        if [ $diff -le 0 ];
        then
            zip "$archive_name" $(find . -print);
            if [ $? -eq 0 ];
            then printf "%-33s generated" "$archive_name"
            else printf "%-33s failed" "$archive_name"
            fi
        else
            printf "$diff seconds before next backup.\n\n"
        fi;

    else printf "warning: auto backup is disabled.\n\n";
    fi;
    echo $(print_centered "Completed");
}

function build_backup() {
    local argc=$((argc - 1));
    local argv=(${*:3:$(($#-1))});
    local backup="backup";

    echo $(print_centered " backup ");

    if [ ! -d "$backup" ];
    then mkdir $backup;
    fi;

    archive_path="$backup/$(date '+%Y%m%d-%Hh%Mm%Ss-%s').zip";
    zip "$archive_path" $(find . -print)
    if [ $? -eq 0 ];
    then echo $(print_centered "Completed");
    else echo $(print_centered "Failed");
    fi;
}

function main {
    local argc=$1;
    local argv=(${*:2:$(($#-1))});

    if [[ $argc -ge 2 && "${argv[0]}" == "cache" ]];
    then
        build_cache $argc ${argv[@]};
        exit 0;
    elif [[ $argc -ge 2 && "${argv[0]}" == "program" ]];
    then
        build_cache $argc ${argv[@]}
        build_program $argc ${argv[@]}
        exit 0
    elif [[ $argc -ge 1 && "${argv[0]}" == "backup" ]];
    then
        build_backup $argc ${argv[@]}
        exit 0
    fi
}

main $# $@
