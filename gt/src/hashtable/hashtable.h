/*
 *  Copyright (c) 2026 Gaël Fortier <gael.fortier.1@ens.etsmtl.ca>
 */

#pragma once
#include "../buf/buf.h"
#include "../trace/trace.h"
#include <stdint.h>

typedef enum {
    GT_HASHTABLE_OK = 0,
    GT_HASHTABLE_INVALID_BUF,
    GT_HASHTABLE_INVALID_CAPACITY,
    GT_HASHTABLE_INVALID_OUT,
    GT_HASHTABLE_INVALID_FUNC,
    GT_HASHTABLE_INVALID_KEY,
    GT_HASHTABLE_INVALID_VALUE,
    GT_HASHTABLE_INVALID_HT,
    GT_HASHTABLE_RESULT_FOUND = 0,
    GT_HASHTABLE_RESULT_ABSENT = 1,
} gt_ht_result;

typedef struct {
    void*  key_ptr;
    size_t key_size;
} gt_ht_key;

typedef union {
    uint64_t u;
    int64_t  i;
    double   f;
    void*    p;
} gt_ht_value;

typedef struct {
    gt_ht_key   key;
    gt_ht_value value;
} gt_ht_keyval;

typedef struct {
    void*      caller;
    uint64_t   index;
    gt_ht_key* key;
} gt_htf_params;

typedef int (*gt_ht_func)(gt_htf_params* params);

typedef struct {
    gt_buf     buf;
    gt_ht_func func;
    size_t     qty;
    size_t     cap;
} gt_hashtable;

int gt_hashtable_init(gt_buf buf, size_t capacity, gt_ht_func hf, gt_hashtable* out);

int gt_hashtable_deinit(gt_hashtable* ht);

int gt_hashtable_set(gt_hashtable* ht, gt_ht_key* key, gt_ht_value* value);

int gt_hashtable_insert(gt_hashtable* ht, gt_ht_key* key, gt_ht_value* value);

int gt_hashtable_get(gt_hashtable* ht, gt_ht_key* key, gt_ht_keyval* out, int* out_found);

int gt_hashtable_remove(gt_hashtable* ht, gt_ht_key* key, gt_ht_keyval* out, int* out_found);

int gt_hashtable_at(gt_hashtable* ht, size_t index, gt_ht_keyval* out);

int gt_hashtable_quantity(gt_hashtable* ht, size_t* out);

int gt_hashtable_full(gt_hashtable* ht, int* out);
