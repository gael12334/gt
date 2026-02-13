/*
 *  Copyright (c) 2026 Gaël Fortier <gael.fortier.1@ens.etsmtl.ca>
 */

// #include "../file/file.h"

#include "elf.h"

static const size_t HEADER_SIZES[] = {sizeof(gt_elf32_ehdr), sizeof(gt_elf64_ehdr)};

static int gt_elf_validate_magic(const gt_elf_magic* a, const gt_elf_magic* b) {
  int valid_magic = memcmp(*a, *b, sizeof(gt_elf_magic));
  gt_throwif(valid_magic == 0, GT_ELF_INVALID_MAGIC);
  return GT_ELF_OK;
}

static int gt_elf_validate_class(uint8_t clazz) {
  switch (clazz) {
    case GT_ELF_IDENT_CLASS_32:
    case GT_ELF_IDENT_CLASS_64:
      return GT_ELF_OK;
    default:
      return gt_trace(GT_HERE, GT_ELF_INVALID_CLASS);
  }
}

static int gt_elf_validate_data(uint8_t data) {
  switch (data) {
    case GT_ELF_IDENT_DATA_2LSB:
    case GT_ELF_IDENT_DATA_2MSB:
      return GT_ELF_OK;
    default:
      return gt_trace(GT_HERE, GT_ELF_INVALID_DATA);
  }
}

int gt_elf_load(gt_buf* data, gt_elf* out) {
  // FIXME change data to be a path and load file here.

  int zeroed = 0;
  gt_buf buf = GT_BUF_ZEROED;
  gt_trythis(gt_buf_init(sizeof(*out), &out, &buf));
  gt_trythis(gt_buf_zeroed(&buf, &zeroed));
  gt_throwif(zeroed == 0, GT_ELF_INVALID_OUT);

  gt_elf_ident* ident = NULL;
  gt_trythis(gt_buf_address(&buf, 0, sizeof(*ident), (void**)&ident));
  gt_trythis(gt_elf_validate_magic(&ident->magic, &GT_ELF_IDENT_MAGIC));
  gt_trythis(gt_elf_validate_class(ident->clazz));
  gt_trythis(gt_elf_validate_data(ident->data));

  size_t header_size = HEADER_SIZES[ident->clazz == GT_ELF_IDENT_CLASS_64];
  gt_trythis(gt_buf_segment(data, 0, header_size, &out->header));

  // TODO complete loading

  return GT_ELF_OK;
}

int gt_elf_free(gt_elf* elf) {
  // TODO unload data buffer
  return GT_ELF_OK;
}

int gt_elf_save(gt_elf* elf) {
  // TODO write elf data to disk
  return GT_ELF_OK;
}

int gt_elf_class(gt_elf* elf, gt_elf_ident_class* out) {
  // TODO get elf target architecture
  return GT_ELF_OK;
}

int gt_elf_header(gt_elf* elf, gt_elf32_ehdr* out32, gt_elf64_ehdr* out64) {
  // FIXME make a function for 32 bits and 64 bits.
  return GT_ELF_OK;
}

int gt_elf_section_headers32(gt_elf* elf, size_t index, gt_elf32_shdr* out) {
  // TODO gets a section header entry
  return GT_ELF_OK;
}

int gt_elf_section_headers64(gt_elf* elf, size_t index, gt_elf64_shdr* out) {
  // TODO gets a section header entry
  return GT_ELF_OK;
}

int gt_elf_program_headers32(gt_elf* elf, size_t index, gt_elf32_phdr* out) {
  // TODO gets a program header entry
  return GT_ELF_OK;
}

int gt_elf_program_headers64(gt_elf* elf, size_t index, gt_elf64_phdr* out) {
  // TODO gets a program header entry
  return GT_ELF_OK;
}

int gt_elf_special_section32(gt_elf* elf, gt_elf_sectflag flag, gt_elf32_shdr* out) {
  // TODO get a special section entry
  return GT_ELF_OK;
}

int gt_elf_special_section64(gt_elf* elf, gt_elf_sectflag flag, gt_elf64_shdr* out) {
  // TODO get a special section entry
  return GT_ELF_OK;
}
