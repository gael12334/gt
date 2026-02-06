/*
 *  Copyright (c) 2026 Gaël Fortier <gael.fortier.1@ens.etsmtl.ca>
 */

#pragma once

#include "../../buf/buf.h"
#include "../../trace/trace.h"
#include "types.h"

int gt_elf_assert_class(gt_buf* buf, int clazz);
int gt_elf_assert_out(void* addr, size_t size);
int gt_elf_assert_arrayref(void* start, void* end, void* ptr, size_t size);
