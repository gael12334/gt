#
# Copyright (c) 2025 Gaël Fortier <gael.fortier.1@ens.etsmtl.ca>
#

gcc ./src/adhoc.c ./src/adhoc_main.c -o adhoc_x_2 -Werror -Wreturn-type -ggdb -O0 -fpatchable-function-entry=50 -fstack-reuse=none -fpcc-struct-return -I/usr/include -L/usr/lib -static-libgcc
