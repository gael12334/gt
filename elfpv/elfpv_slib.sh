#
# Copyright (c) 2025 Gaël Fortier <gael.fortier.1@ens.etsmtl.ca>
#

#
# Arguments
#
# $1 : project output directory path
# $2 : project name
# $3 : project version number

# script import
. ../build/buildtools.sh

# ------------ system --------------
# arguments assertion
SYS_EXPECTED_ARGS=3
SYS_RECIEVED_ARGS=$#
assert_args $SYS_EXPECTED_ARGS $SYS_RECIEVED_ARGS

# output filename path
SYS_OUTP_PATH=$1
SYS_PROJ_NAME=$2
SYS_PROJ_VERS=$3
SYS_OUTP_FILE="${SYS_OUTP_PATH}/${SYS_PROJ_NAME}_${SYS_PROJ_VERS}"
# ----------------------------------

# ----------- build vars -----------
# array of directories to look into for source files
BUILD_SRC_PATH=("src")

# find source files in directories
BUILD_SRC_FILE=$(find $BUILD_SRC_PATH -name "elfpv_*.c" -print)

# GCC compiler options
BUILD_GCC_ARGS=(-ggdb -static -static-libgcc -Werror \
                -Wreturn-type -I/usr/include -L/usr/lib)
LINKR_GCC_ARGS=(-r)

# output file extension
BUILD_OUT_EXTN="o"

# path to output file extension
BUILD_OUT_FILE="${SYS_OUTP_FILE}.${BUILD_OUT_EXTN}"
# -----------------------------------

# -------------- build --------------

build "$SYS_OUTP_PATH" "$BUILD_OUT_FILE" \
      "$BUILD_SRC_PATH" "$BUILD_SRC_FILE" \
      "${BUILD_GCC_ARGS[*]}"
# -----------------------------------
