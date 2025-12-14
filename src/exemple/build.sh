#
# Copyright © 2025 Gaël Fortier <gael.fortier.1@ens.etsmtl.ca>
#

mkdir ./out
gcc -o ./out/program -Werror -Wreturn-type -ggdb $(find . -name '*.c' -print) -I/usr/include -L/usr/lib -static-libgcc # `sdl2-config --cflags --libs` -lSDL2 -lSDL2_image
chmod +x ./out/program
