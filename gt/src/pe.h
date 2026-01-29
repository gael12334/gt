/*
    Copyright (c) 2026 Gaël Fortier <gael.fortier.1@ens.etsmtl.ca>

    References:
    https://www.retroreversing.com/WindowsExecutables
    https://www.openrce.org/reference_library/files/reference/PE%20Format.pdf
    https://learn.microsoft.com/en-us/windows/win32/api/dbghelp/nf-dbghelp-imagedirectoryentrytodata
    https://learn.microsoft.com/en-us/dotnet/api/system.reflection.portableexecutable.pedirectoriesbuilder.copyrighttable?view=net-9.0
*/

#pragma once

#include <stdint.h>

#pragma pack(1)
typedef struct {
    uint16_t signature;
    uint16_t bytes_on_last_page;
    uint16_t pages_in_files;
    uint16_t relocation_entries;
    uint16_t header_size;
    uint16_t minimum_extra;
    uint16_t maximum_extra;
    uint16_t initial_ss;
    uint16_t initial_sp;
    uint16_t checksum;
    uint16_t initial_ip;
    uint16_t initial_cs;
    uint16_t relocation_table_offset;
    uint16_t overlay_number;
    uint8_t reserved[8];
    uint16_t oem_identifier;
    uint16_t oem_information;
    uint8_t reserved2[20];
    uint32_t pointer_to_new_header;
} mz_ehdr;

// COFF HEADER
typedef struct {
    uint16_t machine;
    uint16_t number_of_sections;
    uint32_t time_date_stamp;
    uint32_t pointer_to_symbol_table;
    uint32_t number_of_symbols;
    uint16_t size_of_optional_header;
    uint16_t characteristics;
} pe_fhdr;

typedef struct {
    uint16_t magic;
    uint8_t major_linker_version;
    uint8_t minor_linker_version;
    uint32_t size_of_code;
    uint32_t size_of_init_data;
    uint32_t size_of_uninit_data;
    uint32_t address_of_entrypoint;
    uint32_t base_of_code;
    uint32_t base_of_data;
} pe32_std_ohdr;

typedef struct {
    uint16_t magic;
    uint8_t major_linker_version;
    uint8_t minor_linker_version;
    uint32_t size_of_code;
    uint32_t size_of_init_data;
    uint32_t size_of_uninit_data;
    uint32_t address_of_entrypoint;
    uint32_t base_of_code;
} pe64_std_ohdr;

typedef enum pe_imgdir {
    PE_IODD_ENTRY_EXPORT,
    PE_IODD_ENTRY_IMPORT,
    PE_IODD_ENTRY_RESOURCE,
    PE_IODD_ENTRY_EXCEPTION,
    PE_IODD_ENTRY_SECURITY,
    PE_IODD_ENTRY_BASERELOC,
    PE_IODD_ENTRY_DEBUG,
    PE_IODD_ENTRY_ARCHITECTURE,
    PE_IODD_ENTRY_COPYRIGHT = PE_IODD_ENTRY_ARCHITECTURE,
    PE_IODD_ENTRY_GLOBALPTR,
    PE_IODD_ENTRY_TLS,
    PE_IODD_ENTRY_LOAD_CONFIG,
    PE_IODD_ENTRY_BOUND_IMPORT,
    PE_IODD_ENTRY_IAT,
    PE_IODD_ENTRY_DELAY_IMPORT,
    PE_IODD_ENTRY_COM_DESCRIPTOR,
} pe_imgdir;

typedef struct {
    uint32_t virutal_address;
    uint32_t size;
} pe_datadir;

typedef struct {
    uint32_t image_base;
    uint32_t section_alignment;
    uint32_t file_alignment;
    uint16_t major_os_version;
    uint16_t minor_os_version;
    uint16_t major_subsys_version;
    uint16_t minor_subsys_version;
    uint32_t wind32_version_value;
    uint32_t size_of_image;
    uint32_t size_of_header;
    uint32_t checksum;
    uint16_t subsys;
    uint16_t dll_characteristics;
    uint32_t size_of_stack_reserve;
    uint32_t size_of_stack_commit;
    uint32_t size_of_heap_reserve;
    uint32_t size_of_heap_commit;
    uint32_t loader_flags;
    uint32_t number_of_rva_and_sizes;
    pe_datadir data_directories[16];
} pe32_win_ohdr;

typedef struct {
    uint64_t image_base;
    uint32_t section_alignment;
    uint32_t file_alignment;
    uint16_t major_os_version;
    uint16_t minor_os_version;
    uint16_t major_subsys_version;
    uint16_t minor_subsys_version;
    uint32_t wind32_version_value;
    uint32_t size_of_image;
    uint32_t size_of_header;
    uint32_t checksum;
    uint16_t subsys;
    uint16_t dll_characteristics;
    uint64_t size_of_stack_reserve;
    uint64_t size_of_stack_commit;
    uint64_t size_of_heap_reserve;
    uint64_t size_of_heap_commit;
    uint32_t loader_flags;
    uint32_t number_of_rva_and_sizes;
    pe_datadir data_directories[16];
} pe64_win_ohdr;

typedef struct {
    uint32_t signature;
    pe_fhdr file_header;
    pe32_std_ohdr coff_header;
    pe32_win_ohdr windows_header;
} pe32_nthdr;

typedef struct {
    uint32_t signature;
    pe_fhdr file_header;
    pe64_std_ohdr coff_header;
    pe64_win_ohdr windows_header;
} pe64_nthdr;

#define PE_SHDR_SIZE_OF_NAME (8)
typedef struct {
    uint8_t name[PE_SHDR_SIZE_OF_NAME];
    union {
        uint32_t physical_address;
        uint32_t virutal_size;
    } misc;
    uint32_t virtual_address;
    uint32_t size_of_raw_data;
    uint32_t pointer_to_raw_data;
    uint32_t pointer_to_relocations;
    uint32_t pointer_to_line_numbers;
    uint16_t number_of_relocations;
    uint16_t number_of_line_numbers;
    uint32_t characteristics;
} pe_shdr;

#pragma pop
