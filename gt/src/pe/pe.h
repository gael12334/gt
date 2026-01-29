/*
 *  Copyright (c) 2026 Gaël Fortier <gael.fortier.1@ens.etsmtl.ca>
 */

#pragma once
#include "coff.h"
#include "mz.h"
#pragma push pack(1)

typedef struct {
    uint8_t  name[8];
    uint32_t virtual_size;
    uint32_t virtual_address;
    uint32_t size_of_raw_data;
    uint32_t ptr_to_raw_data;
    uint32_t ptr_to_relocs;
    uint32_t ptr_to_line_nb;
    uint16_t nb_of_relocs;
    uint16_t nb_of_line_nb;
    uint32_t flags;
} gt_PESECTION;

typedef struct {
    gt_COFFHEADER coff_header;
    gt_PESECTION  pe_sections[];
} gt_PEHEADER;

typedef struct {
    gt_COFFHEADER   coff_header;
    gt_COFF32STDEXT coff_stdext;
    gt_PESECTION    pe_sections[];
} gt_PEHEADER_COFF32STDEXT;

typedef struct {
    gt_COFFHEADER    coff_header;
    gt_COFF32STDEXT  coff_stdext;
    gt_COFF32WINEXT  coff_winext;
    gt_DATADIRECTORY win32_datadir[16];
    gt_PESECTION     pe_sections[];
} gt_PEHEADER_COFF32WINEXT;

typedef struct {
    gt_COFFHEADER   coff_header;
    gt_COFF64STDEXT coff_stdext;
    gt_PESECTION    pe_sections[];
} gt_PEHEADER_COFF64STDEXT;

typedef struct {
    gt_COFFHEADER    coff_header;
    gt_COFF64STDEXT  coff_stdext;
    gt_COFF64WINEXT  coff_winext;
    gt_DATADIRECTORY pe_datadir[16];
    gt_PESECTION     pe_sections[];
} gt_PEHEADER_COFF64WINEXT;

#pragma pop
