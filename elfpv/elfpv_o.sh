#
# Copyright (c) 2025 Gaël Fortier <gael.fortier.1@ens.etsmtl.ca>
#

# script import
. ../build/buildtools.sh

# ------------ system --------------
# arguments assertion
SYS_EXPECTED_ARGS=4
SYS_RECIEVED_ARGS=$#
assert_args $SYS_EXPECTED_ARGS $SYS_RECIEVED_ARGS

# output filename path
SYS_OUTP_PATH=$1  # project output directory path
SYS_PROJ_NAME=$2  # project name
SYS_PROJ_VERS=$3  # project version number
SYS_PROJ_PBSH=$4  # project publish directory
SYS_OUTP_FILE="${SYS_OUTP_PATH}/${SYS_PROJ_NAME}_${SYS_PROJ_VERS}"
# ----------------------------------

# ------------ prebuild ------------
# insert user code here

# ----------------------------------

# -------------- build --------------
# output file extension
BUILD_OUT_EXTN="o"

# path to output file extension
BUILD_OUT_FILE="${SYS_OUTP_FILE}.${BUILD_OUT_EXTN}"

# array of directories to look into for source files
BUILD_SRC_PATH=("src")

# find source files in directories
BUILD_SRC_FILE=$(find $BUILD_SRC_PATH -name "elfpv_*.c" -print)

# GCC compiler options
BUILD_GCC_ARGS=(-ggdb -static -static-libgcc -Werror \
                -Wreturn-type -I/usr/include -L/usr/lib)
LINKR_GCC_ARGS=(-r)

# -----------------------------------
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
# directories to find files
PBLSH_HDR_PATH=("$BUILD_SRC_PATH")
PBLSH_LIB_PATH=("$SYS_OUTP_PATH/$BUILD_SRC_PATH")
PBLSH_BIN_PATH=("")

# find files in directories (for publishing)
PBLSH_HDR_FILE=$(find "${PBLSH_HDR_PATH[*]}" -name "elfpv_*.h" -print)
PBLSH_LIB_FILE=$(find "${PBLSH_LIB_PATH[*]}" -name "elfpv_*.o" -print)
PBLSH_BIN_FILE=$()

# -----------------------------------
echo "$SYS_PROJ_NAME"
echo "$SYS_PROJ_VERS"
echo "$SYS_PROJ_PBSH"
echo "${PBLSH_HDR_FILE[*]}"
echo "${PBLSH_LIB_FILE[*]}"
echo "${PBLSH_BIN_FILE[*]}"

publish "$SYS_PROJ_NAME" \
        "$SYS_PROJ_VERS" \
        "$SYS_PROJ_PBSH" \
        "${PBLSH_HDR_FILE[*]}" \
        "${PBLSH_LIB_FILE[*]}" \
        "${PBLSH_BIN_FILE[*]}"
# -----------------------------------
