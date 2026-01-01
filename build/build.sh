#
# Copyright (c) 2025 Gaël Fortier <gael.fortier.1@ens.etsmtl.ca>
#

gcc -o bt buildtools.c -ggdb -static -static-libgcc -Werror -Wreturn-type -I/usr/include -L/usr/lib
if [ $? -ne 0 ]; then exit; fi
chmod +x bt
