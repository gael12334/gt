/*
    Copyright (c) 2025 Gaël Fortier <gael.fortier.1@ens.etsmtl.ca>
*/

#pragma once
#include <stdio.h>

typedef long _gt_long;
typedef double _gt_dble;
typedef void* _gt_pntr;
typedef const char* _gt_strg;
typedef char _gt_bf20[20];
typedef char* _gt_bufn;

typedef union {
    _gt_long _long;
    _gt_dble _dble;
    _gt_pntr _pntr;
    _gt_strg _strg;
} _gt_poly;

typedef enum {
    _gt_long_type,
    _gt_dble_type,
    _gt_pntr_type,
    _gt_strg_type,
    _gt_type_nmbr,
} _gt_type;

typedef enum {
    _gt_cond_equl, // ==
    _gt_cond_nteq, // !=
    _gt_cond_grth, // >
    _gt_cond_lrth, // <
    _gt_cond_greq, // >=
    _gt_cond_lreq, // <=
    _gt_cond_nmbr, // nb of const in enum
} _gt_cond;

typedef struct {
    _gt_poly poly_x, poly_y;
    _gt_strg expr_x, expr_y;
    _gt_cond cond;
    _gt_type type;
} _gt_asrt;

int _gt_set_func(_gt_strg Func);
int _gt_set_case(_gt_strg Case);
int _gt_end_case(void);
int _gt_end_func(void);
int _gt_chk_long(_gt_long val1, _gt_strg exp1, _gt_long val2, _gt_strg exp2, _gt_cond cond);
int _gt_chk_dble(_gt_dble val1, _gt_strg exp1, _gt_dble val2, _gt_strg exp2, _gt_cond cond);
int _gt_chk_pntr(_gt_pntr val1, _gt_strg exp1, _gt_pntr val2, _gt_strg exp2, _gt_cond cond);
int _gt_chk_strg(_gt_strg val1, _gt_strg exp1, _gt_strg val2, _gt_strg exp2, _gt_cond cond);
void _gt_set_lctn(_gt_strg file, _gt_long line);

#define gt_assert_long(x, y, cond) \
    _gt_set_lctn(__FILE__, __LINE__);\
    if(!_gt_chk_long((_gt_long)x, #x, (_gt_long)y, #y, cond)) \
        continue;

#define gt_assert_dble(x, y, cond) \
    _gt_set_lctn(__FILE__, __LINE__);\
    if(!_gt_chk_dble((_gt_dble)x, #x, (_gt_dble)y, #y, cond)) \
        continue;

#define gt_assert_pntr(x, y, cond) \
    _gt_set_lctn(__FILE__, __LINE__);\
    if(!_gt_chk_pntr((_gt_pntr)x, #x, (_gt_pntr)y, #y, cond)) \
        continue;

#define gt_assert_strg(x, y, cond) \
    _gt_set_lctn(__FILE__, __LINE__);\
    if(!_gt_chk_strg((_gt_strg)x, #x, (_gt_strg)y, #y, cond)) \
        continue;

#define gt_test(name) \
    void _gt_test_fn_##name(_gt_strg ttnm)

#define gt_case(name) \
    for(int _ = _gt_set_case(#name); _ == 0; _ = _gt_end_case())

#define gt_run(name) \
    _gt_set_func(#name); \
    _gt_test_fn_##name(#name); \
    _gt_end_func();

#define GT_EQUALS_ _gt_cond_equl // ==
#define GT_DIFFERS _gt_cond_nteq // !=
#define GT_GREATER _gt_cond_grth // >
#define GT_SMALLER _gt_cond_lrth // <
#define GT_ATLEAST _gt_cond_greq // >=
#define GT_AT_MOST _gt_cond_lreq // <=

#ifndef GT_TEST_ENABLED
#define gt_mockable __attribute__((weak))
#endif
