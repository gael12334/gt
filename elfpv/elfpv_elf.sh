#
# Copyright (c) 2025 Gaël Fortier <gael.fortier.1@ens.etsmtl.ca>
#

# script import
. ../build/buildtools.sh

# ------------ system --------------
# arguments assertion
SYS_EXPECTED_ARGS=3
SYS_RECIEVED_ARGS=$#
assert_args $SYS_EXPECTED_ARGS $SYS_RECIEVED_ARGS

# output filename path
SYS_OUTP_PATH=$1  # project output directory path
SYS_PROJ_NAME=$2  # project name
SYS_PROJ_VERS=$3  # project version number
SYS_PBLSH_DIR=$4  # directory to "publish" project
SYS_OUTP_FILE="${SYS_OUTP_PATH}/${SYS_PROJ_NAME}_${SYS_PROJ_VERS}"
# ----------------------------------

# ------------ prebuild ------------
# insert user code here

# ----------------------------------

# -------------- build -------------
# output file extension
BUILD_OUT_EXTN="elf"

# path to output file extension
BUILD_OUT_FILE="${SYS_OUTP_FILE}.${BUILD_OUT_EXTN}"

# array of directories to look into for source files
BUILD_SRC_PATH=("src")

# find source files in directories
BUILD_SRC_FILE=$(find $BUILD_SRC_PATH -name "*.c" -print)

# GCC compiler options
BUILD_GCC_ARGS=(-ggdb -static -static-libgcc -Werror \
                -Wreturn-type -I/usr/include -L/usr/lib)
LINKR_GCC_ARGS=()

# start build
build "$SYS_OUTP_PATH" \
      "$BUILD_OUT_FILE" \
      "$BUILD_SRC_PATH" \
      "$BUILD_SRC_FILE" \
      "${LINKR_GCC_ARGS[*]}" \
      "${BUILD_GCC_ARGS[*]}"
# -----------------------------------

# ------------ postbuild ------------
# insert user code here

# ----------------------------------

# ------------ publish --------------
# find files in directories (for publishing)
PBLSH_HDR_FILE=$()
PBLSH_LIB_FILE=$()
PBLSH_BIN_FILE=$($BUILD_OUT_FILE)

# -----------------------------------
publish "$SYS_PROJ_NAME" \
        "$SYS_PROJ_VERS" \
        "$SYS_PBLSH_DIR" \
        "${PBLSH_HDR_FILE[*]}" \
        "${PBLSH_LIB_FILE[*]}" \
        "${PBLSH_BIN_FILE[*]}"
# -----------------------------------
