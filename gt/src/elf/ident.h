/*
 *  Copyright (c) 2026 Gaël Fortier <gael.fortier.1@ens.etsmtl.ca>
 */

#pragma once
#include <stdint.h>
#pragma push pack(1)

/*
 * Reference :
 *
 * elf(5) - Linux manual page. (n.d.). Man7.org.
 * https://man7.org/linux/man-pages/man5/elf.5.html
 */

typedef struct {
    uint8_t ei_mag0;
    uint8_t ei_mag1;
    uint8_t ei_mag2;
    uint8_t ei_mag3;
    uint8_t ei_class;
    uint8_t ei_data;
    uint8_t ei_version;
    uint8_t ei_osabi;
    uint8_t ei_abiversion;
    uint8_t ei_pad[7];
} gt_elf_ident;

#pragma pop
