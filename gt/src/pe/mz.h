/*
 *  Copyright (c) 2026 Gaël Fortier <gael.fortier.1@ens.etsmtl.ca>
 */

#pragma once
#include <stdint.h>
#pragma push pack(1)

typedef struct {
    uint16_t magic;
    uint16_t byte_nb_last_page;
    uint16_t number_of_pages;
    uint16_t number_of_relocs;
    uint16_t size_of_header;
    uint16_t minimum_alloc;
    uint16_t maximum_alloc;
    uint16_t initial_ss;
    uint16_t initial_sp;
    uint16_t checksum;
    uint16_t initial_ip;
    uint16_t initial_cs;
    uint16_t reloc_address;
    uint16_t overlay;
    uint16_t reserved[4];
    uint16_t oemid;
    uint16_t oeminfo;
    uint16_t reserved2[10];
    uint32_t offset_to_coff;
} gt_MZHEADER;

#pragma pop
