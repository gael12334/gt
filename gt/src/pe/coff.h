/*
 *  Copyright (c) 2026 Gaël Fortier <gael.fortier.1@ens.etsmtl.ca>
 */

#pragma once
#include <stdint.h>
#pragma push pack(1)

typedef struct {
    uint32_t virtual_addr;
    uint32_t size;
} gt_DATADIRECTORY;

typedef struct {
    uint32_t flags;
    uint32_t timestamp;
    uint16_t major;
    uint16_t minor;
    uint32_t type;
    uint32_t size_of_data;
    uint32_t address_of_raw_data;
    uint32_t pointer_to_raw_data;
} gt_DEBUGDIRECTORY;

typedef struct {
    uint16_t machine;
    uint16_t number_of_section;
    uint32_t time_date_stamp;
    uint32_t pointer_to_symbol_table;
    uint32_t number_of_symbol_table;
    uint16_t size_of_optional_header;
    uint16_t characteristics;
} gt_COFFHEADER;

typedef struct {
    gt_COFFHEADER header;
    uint16_t      magic;
    uint8_t       major_linker_version;
    uint8_t       minor_linker_version;
    uint32_t      size_of_code;
    uint32_t      size_of_initialzed_data;
    uint32_t      size_of_uninitialized_data;
    uint32_t      address_of_entry_point;
    uint32_t      base_of_code;
    uint32_t      base_of_data;
} gt_COFF32STDEXT;

typedef struct {
    uint32_t         image_base;
    uint32_t         section_alignment;
    uint32_t         file_alignment;
    uint16_t         major_operating_system_version;
    uint16_t         minor_operating_system_version;
    uint16_t         major_image_version;
    uint16_t         minor_image_version;
    uint16_t         major_subsystem_version;
    uint16_t         minor_subsystem_version;
    uint32_t         win32_version_value;
    uint32_t         size_of_image;
    uint32_t         size_of_header;
    uint32_t         checksum;
    uint16_t         subsystem;
    uint16_t         dll_characteristics;
    uint32_t         size_of_stack_reserve;
    uint32_t         size_of_stack_commit;
    uint32_t         size_of_heap_reserve;
    uint32_t         size_of_heap_commit;
    uint32_t         loader_flags;
    uint32_t         number_of_datadir;
    gt_DATADIRECTORY data_directories[16];
} gt_COFF32WINEXT;

typedef struct {
    gt_COFFHEADER header;
    uint16_t      magic;
    uint8_t       major_linker_version;
    uint8_t       minor_linker_version;
    uint32_t      size_of_code;
    uint32_t      size_of_initialzed_data;
    uint32_t      size_of_uninitialized_data;
    uint32_t      address_of_entry_point;
    uint32_t      base_of_code;
} gt_COFF64STDEXT;

typedef struct {
    uint64_t         image_base;
    uint32_t         section_alignment;
    uint32_t         file_alignment;
    uint16_t         major_operating_system_version;
    uint16_t         minor_operating_system_version;
    uint16_t         major_image_version;
    uint16_t         minor_image_version;
    uint16_t         major_subsystem_version;
    uint16_t         minor_subsystem_version;
    uint32_t         win32_version_value;
    uint32_t         size_of_image;
    uint32_t         size_of_header;
    uint32_t         checksum;
    uint16_t         subsystem;
    uint16_t         dll_characteristics;
    uint64_t         size_of_stack_reserve;
    uint64_t         size_of_stack_commit;
    uint64_t         size_of_heap_reserve;
    uint64_t         size_of_heap_commit;
    uint32_t         loader_flags;
    uint32_t         number_of_datadir;
    gt_DATADIRECTORY data_directories[16];
} gt_COFF64WINEXT;

typedef struct {
    uint32_t virtual_address;
    uint32_t symbol_table_index;
    uint16_t type;
} gt_COFFRELOC;

typedef union {
    uint8_t shortname[8];
    struct {
        uint32_t zeros;
        uint32_t offset;
    };
} gt_COFFSHORTNAME;

typedef struct {
    gt_COFFSHORTNAME name;
    uint32_t         value;
    uint16_t         number_of_section;
    uint16_t         type;
    uint8_t          storage_class;
    uint8_t          number_of_auxsym;
} gt_COFFSYMTAB;

typedef struct {
    uint32_t tag_index;
    uint32_t total_size;
    uint32_t pointer_to_line_nb;
    uint32_t pointer_to_next_fn;
    uint16_t unused;
} gt_COFFFUNCDEF;

typedef struct {
    uint32_t unused0;
    uint16_t line_number;
    uint8_t  unused1[6];
    uint32_t pointer_to_next_fn;
    uint16_t unused2;
} gt_COFFFUNCBEGIN;

typedef struct {
    uint32_t unused0;
    uint16_t line_number;
    uint8_t  unused1[6];
    uint32_t pointer_to_next_fn;
    uint16_t unused2;
} gt_COFFFUNCEND;

typedef struct {
    uint32_t tag_index;
    uint32_t flags;
    uint8_t  unused[10];
} gt_COFFWEAKEXTERNAL;

typedef struct {
    uint8_t filename[18];
} gt_COFFFILES;

typedef struct {
    uint32_t length;
    uint16_t number_of_relocs;
    uint16_t number_of_line_nb;
    uint32_t checksum;
    uint16_t number;
    uint8_t  selection;
    uint8_t  unused[3];
} gt_COFFSECTIONDEF;

typedef struct {
    uint8_t  aux_type;
    uint8_t  reserved0;
    uint32_t symbol_table_index;
    uint8_t  reserved1[12];
} gt_COFFCLRTOKEN;

typedef struct {
    uint32_t size;
} gt_COFFSTRINGTABLE;

typedef struct {
    uint32_t length;
    uint16_t revision;
    uint32_t cert_type;
} gt_COFFATTRIBCERTTABLE;

typedef struct {
    uint32_t attributes;
    uint32_t name;
    uint32_t module;
    uint32_t address_table;
    uint32_t name_table;
    uint32_t bound_delay_table;
    uint32_t unload_delay_table;
    uint32_t timestamp;
} gt_COFFDELAYIMPORTTABLE;

#pragma pop
