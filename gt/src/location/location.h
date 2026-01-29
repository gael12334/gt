/*
 *  Copyright (c) 2026 Gaël Fortier <gael.fortier.1@ens.etsmtl.ca>
 */

#pragma once

typedef struct {
    const char*  file;
    unsigned int line;
} gt_location;

#define GT_HERE           \
    (gt_location)         \
    {                     \
        .file = __FILE__, \
        .line = __LINE__  \
    }
