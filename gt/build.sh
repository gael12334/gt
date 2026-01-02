#
# Copyright (c) 2026 Gaël Fortier <gael.fortier.1@ens.etsmtl.ca>
#

# ---------------------
# Assertion
function assert() {
    if [ $1 -ne 0 ];
    then exit;
    fi
}

# Creates directories from a path string
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

# Removes the output directory and its content.
function clean_output() {
    local output=$1
    local project_name=$2
    printf "[$project_name]: Removing output directory ($output) \n"
    rm -rf $output
    assert $?
    printf "[$project_name]: :... Compeleted \n"
}

# Creates a backup of the source code
function backup_source() {
    local source=$1
    local directory=$2
    local name=$3
    local archive=$(date '+%Y%m%d-%Hh%Mm%Ss-%s')

    if [ ! -d $directory ]; then
        mkdir $directory
        assert $?
    fi
    printf "[$name]: Creating backup of the '$source' directory \n"
    local archive_path="./$directory/$archive.zip"
    zip "$archive_path" $(find $source -print)
    assert $?
    printf "[$name]: :... Completed backup (saved at '$archive_path') \n"
}

# Check if source file was modified after last object
# file generated
function precompile() {
    local name=$1
    local object=$2
    local source=$3
    local linker_args=$5
    local gcc_args=$4

    if [ -f $object ]; then
        src_last_changed=$(date -r $source "+%s")
        obj_last_changed=$(date -r $object "+%s")

        if [ $src_last_changed -lt $obj_last_changed ]; then
            printf "[$name]: $source \n"
            printf "[$name]: :... Up to date \n"
            return
        fi
    fi

    compile "$name" "$object" "$source" "$linker_args" "$gcc_args"
}

# Compilation
R_compiled=0
function compile() {
    # params
    local name=$1
    local output=$2
    local sources=$3
    local linker_args=$4
    local gcc_args=$5

    # command
    local command="gcc -o $output $sources $linker_args $gcc_args"
    printf "[$name]: $command\n"

    # execution
    $($command)
    if [ $? -eq 0 ]; then
        printf "[$name]: :... Succeeded\n"
    else
        printf "[$name]: :... Failed\n"
        exit 1
    fi

    R_compiled=$(($R_compiled + 1))
}

# ---------------------
#     ENTRY POINT
# ---------------------
# project name
PROJECT_NAME="gt"
# backup directory
BACKUP_DIRECTORY="bck"
# output directory
OUT_DIRECTORY="out"
# precompiled object files
OBJECT_FILES=$()
# source code directory
SOURCE_DIRECTORY="src"
# source code files
SOURCE_FILES=$(find "$SOURCE_DIRECTORY" -name "*.c" -printf "%p ")
# GCC arguments for precompiled relocatable object files (.o)
OBJECTS_GCC_ARGS="-ggdb -static -static-libgcc -Werror -Wreturn-type -I/usr/include -L/usr/lib -DPRECOMPILE"
OBJECTS_GCC_LNKR="-r"
# GCC arguments for executable file (elf)
EXECUTABLE_GCC_ARGS="-ggdb -static -static-libgcc -Werror -Wreturn-type -I/usr/include -L/usr/lib -DEXECUTABLE"
EXECUTABLE_GCC_LNKR=""
# Path of output files
LIB_A_PATH="$OUT_DIRECTORY/lib$PROJECT_NAME.a"
ELF_PATH="$OUT_DIRECTORY/$PROJECT_NAME.elf"
# toogle to compile project as library
# (will ignore object files that are executable)
TOGGLE_LIBRARY=0

# ---------------------
# checks arguments
if [[ $# -ge 1 && "$1" == "clean" ]]; then
    clean_output "$OUT_DIRECTORY" "$PROJECT_NAME"
    exit 0
elif [[ $# -ge 1 && "$1" == "backup" ]]; then
    cp ./build.sh ./src/build-backup.sh
    printf "#\n# Copyright (c) 2026 Gaël Fortier <gael.fortier.1@ens.etsmtl.ca>\n#\nmv ./build-backup.sh ../build.sh" > ./src/setup.sh
    chmod +x ./src/setup.sh
    backup_source "$SOURCE_DIRECTORY" "$BACKUP_DIRECTORY" "$PROJECT_NAME"
    rm ./src/build-backup.sh
    rm ./src/setup.sh
    exit 0
elif [[ $# -ge 1 && "$1" == "library" ]]; then
    TOGGLE_LIBRARY=1
    printf "[$PROJECT_NAME]: Target \n"
    printf "[$PROJECT_NAME]: :... Library \n"
    printf "[$PROJECT_NAME]: ================================== \n"
elif [[ $# -ge 1 && "$1" == "exec" ]]; then
    printf "[$PROJECT_NAME]: Target \n"
    printf "[$PROJECT_NAME]: :... Executable \n"
    printf "[$PROJECT_NAME]: ================================== \n"
else
    printf "arguments:\n"
    printf "clean     (deletes $OUT_DIRECTORY)\n"
    printf "backup    (zips $SOURCE_DIRECTORY)\n"
    printf "library   (creates $LIB_A_PATH)\n"
    printf "exec      (creates $ELF_PATH)\n"
    printf "help      (this output)\n"
    exit 0
fi

# ---------------------

# precompiles every .c file into a .o file
objfile=""
for file in $SOURCE_FILES; do
    # path without source dir
    wosrcdir=${file##$SOURCE_DIRECTORY}

    # path without extension
    woext=${wosrcdir%%".c"}

    # creates the path of the object file
    objfile="$OUT_DIRECTORY$woext.o"

    # Directories of the object file path.
    objdir=${objfile%%$(basename $objfile)}
    create_path $objdir

    # Adding the object file path in the list
    OBJECT_FILES+=($objfile)

    # precompiling relocatable object files
    precompile "$PROJECT_NAME" "$objfile" "$file" "$OBJECTS_GCC_LNKR" "$OBJECTS_GCC_ARGS"
done

# ---------------------
if [[ $TOGGLE_LIBRARY -eq 1 && (! -f "$LIB_A_PATH" ||  $R_compiled -gt 0) ]]; then
    libfiles=()
    echo ${OBJECT_FILES[@]}
    for objfile in ${OBJECT_FILES[@]}; do
        elf_entry=$(./gt.elf elf "$objfile" header | grep -Eo "e_entry +[0-9]+")
        entry_offset=$(echo $elf_entry | grep -Eo "[0-9]+")
        if [ $entry_offset -eq 0 ]; then
            libfiles+=($objfile)
        fi
    done
    ar rcs "$LIB_A_PATH" "$libfiles"
    assert $?
    exit 0
elif [[ ! -f "$ELF_PATH" || $R_compiled -gt 0 ]]; then
    objfiles=$(echo "${OBJECT_FILES[@]}")
    compile "$PROJECT_NAME" "$ELF_PATH" "$objfiles" "$LIBRARY_GCC_LNKR" "$LIBRARY_GCC_ARGS"
    exit 0
fi
printf "[$PROJECT_NAME]: ================================== \n"
printf "[$PROJECT_NAME]: $R_compiled object files generated \n"
printf "[$PROJECT_NAME]: :... Up to date \n"
