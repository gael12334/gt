/*
 *  Copyright (c) 2026 Gaël Fortier <gael.fortier.1@ens.etsmtl.ca>
 */

#include "hashtable.h"

static int gt_hashtable_compare_keys(gt_ht_key* keyA, gt_ht_key* keyB, int* out)
{
    if (keyA->key_ptr == NULL || keyB->key_ptr == NULL) {
        *out = 0;
        return GT_HASHTABLE_OK;
    }

    gt_buf bufA = { 0 };
    GT_TRYTHIS(gt_buf_init(keyA->key_size, keyA->key_ptr, &bufA));

    gt_buf bufB = { 0 };
    GT_TRYTHIS(gt_buf_init(keyB->key_size, keyB->key_ptr, &bufB));

    int key_equal = 0;
    GT_TRYTHIS(gt_buf_equals(&bufA, &bufB, &key_equal));

    *out = key_equal;
    return GT_HASHTABLE_OK;
}

static int gt_hashtable_get_keyvalue(gt_hashtable* ht, size_t index, gt_ht_keyval** out)
{
    size_t size = sizeof(**out);
    size_t offset = index * size;
    GT_TRYTHIS(gt_buf_effective(&ht->buf, offset, size, (void**)out));
    return GT_HASHTABLE_OK;
}

int gt_hashtable_init(gt_buf buf, size_t capacity, gt_ht_func hf, gt_hashtable* out)
{
    GT_THROWIF(hf == NULL, GT_HASHTABLE_INVALID_FUNC);
    GT_THROWIF(out == NULL, GT_HASHTABLE_INVALID_OUT);
    GT_THROWIF(capacity == 0, GT_HASHTABLE_INVALID_CAPACITY);

    size_t buf_size = 0;
    GT_TRYTHIS(gt_buf_size(&buf, &buf_size));
    GT_THROWIF(buf_size != capacity * sizeof(gt_ht_keyval), GT_HASHTABLE_INVALID_BUF);

    out->buf = buf;
    out->cap = capacity;
    out->qty = 0;
    out->func = hf;

    return GT_HASHTABLE_OK;
}

int gt_hashtable_deinit(gt_hashtable* ht)
{
    GT_THROWIF(ht == NULL, GT_HASHTABLE_INVALID_HT);
    memset(ht, 0, sizeof(*ht));
    return GT_HASHTABLE_OK;
}

int gt_hashtable_set(gt_hashtable* ht, gt_ht_key* key, gt_ht_value* value)
{
    GT_THROWIF(ht == NULL, GT_HASHTABLE_INVALID_HT);
    GT_THROWIF(key == NULL, GT_HASHTABLE_INVALID_KEY);
    GT_THROWIF(value == NULL, GT_HASHTABLE_INVALID_VALUE);
    GT_THROWIF(key->key_size == 0, GT_HASHTABLE_INVALID_KEY);
    GT_THROWIF(key->key_ptr == NULL, GT_HASHTABLE_INVALID_KEY);
    return GT_HASHTABLE_OK;
}

int gt_hashtable_insert(gt_hashtable* ht, gt_ht_key* key, gt_ht_value* value)
{
    return GT_HASHTABLE_OK;
}

int gt_hashtable_get(gt_hashtable* ht, gt_ht_key* key, gt_ht_keyval* out, int* out_found)
{
    GT_THROWIF(ht == NULL, GT_HASHTABLE_INVALID_HT);
    GT_THROWIF(key == NULL, GT_HASHTABLE_INVALID_KEY);
    GT_THROWIF(out == NULL, GT_HASHTABLE_INVALID_OUT);
    GT_THROWIF(out_found == NULL, GT_HASHTABLE_INVALID_OUT);
    GT_THROWIF(key->key_size == 0, GT_HASHTABLE_INVALID_KEY);
    GT_THROWIF(key->key_ptr == NULL, GT_HASHTABLE_INVALID_KEY);

    gt_htf_params params;
    params.caller = ht;
    params.index = 0;
    params.key = key;
    GT_TRYTHIS(ht->func(&params));

    int           looped = 0;
    int           key_equal = 0;
    uint64_t      index = params.index;
    gt_ht_keyval* ht_kv = NULL;

    while (key_equal == 0 && looped == 0) {
        GT_TRYTHIS(gt_hashtable_get_keyvalue(ht, index, &ht_kv));
        GT_TRYTHIS(gt_hashtable_compare_keys(&ht_kv->key, key, &key_equal));
        index++;
        index *= (index < ht->cap);
        looped += (index == params.index);
    }

    *out = *ht_kv;
    *out_found = key_equal;
    return GT_HASHTABLE_OK;
}

int gt_hashtable_remove(gt_hashtable* ht, gt_ht_key* key, gt_ht_keyval* out, int* out_found)
{
    return GT_HASHTABLE_OK;
}

int gt_hashtable_at(gt_hashtable* ht, size_t index, gt_ht_keyval* out)
{
    return GT_HASHTABLE_OK;
}

int gt_hashtable_quantity(gt_hashtable* ht, size_t* out)
{
    return GT_HASHTABLE_OK;
}
