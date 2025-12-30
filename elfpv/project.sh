#
# Copyright (c) 2025 Gaël Fortier <gael.fortier.1@ens.etsmtl.ca>
#


PROJECT_TAG="elfpv"
PROJECT_VER="1"
PROJECT_OUT="out"
PROJECT_CMD=("elfpv_slib.sh" "elfpv_exec.sh")
#PROJECT_PKG=("../pkg")
#PROJECT_DEP=()

./elfpv_exec.sh $PROJECT_OUT $PROJECT_TAG $PROJECT_VER
./elfpv_slib.sh $PROJECT_OUT $PROJECT_TAG $PROJECT_VER
