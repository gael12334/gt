/*
 *  Copyright (c) 2026 Gaël Fortier <gael.fortier.1@ens.etsmtl.ca>
 */

#pragma once
#include "../../buf/buf.h"
#include "../../trace/trace.h"
#include "shdr.h"
#include "types.h"

/*************************
 * Module 32 bit functions
 *************************/

int gt_elf32_get_str_segment(gt_buf* buf, gt_elf32_shdr* shdr, gt_buf* out_segment);
