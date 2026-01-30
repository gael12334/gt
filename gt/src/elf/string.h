/*
 * Copyright (c) 2026 Gaël Fortier <gael.fortier.1@ens.etsmtl.ca>
 */

#pragma once
#include <stddef.h>

/*
 * Reference :
 *
 * elf(5) - Linux manual page. (n.d.). Man7.org.
 * https://man7.org/linux/man-pages/man5/elf.5.html
 *
 * Matz, M., Hubička, J., Jaeger, A., & Mitchell, M. (2012).
 * System V Application Binary Interface AMD64 Architecture Processor Supplement Draft Version.
 * https://refspecs.linuxbase.org/elf/x86_64-abi-0.99.pdf
 *
 */

const char gt_elf_phdrtype_str[][8] = {
    "NULL",
    "LOAD",
    "DYNAMIC",
    "INTERP",
    "NOTE",
    "SHLIB",
    "PHDR",
    "TLS"
};
