/*
    Copyright (c) 2025 Gaël Fortier <gael.fortier.1@ens.etsmtl.ca>
*/

#include "gt.h"
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

struct {
    // case section
    _gt_strg Case;
    _gt_strg File;
    _gt_strg Exp1;
    _gt_strg Exp2;
    _gt_bf20 Val1;
    _gt_bf20 Val2;
    _gt_strg Str1;
    _gt_strg Str2;
    _gt_cond Cond;
    _gt_long Rslt;
    _gt_long Line;
    // function section
    _gt_strg Func;
    _gt_long NLen;
    _gt_long PdSt;
    _gt_long Pass;
    _gt_long Fail;
    // module section
    _gt_strg Mdul;
    _gt_long F_ok;
    _gt_long Ferr;
} _gt = {0};

static int mesg_flag(_gt_long fore, _gt_long newl) {
    newl &= 1;
    fore <<= 1;
    return fore | newl;
}

static int msex_flag(_gt_long fore, _gt_long back, _gt_long newl) {
    newl &= 1;
    fore <<= 1;
    back <<= 9;
    return back | fore | newl;
}

static void show_padd() {
    if(_gt.PdSt == 1) {
        char padd[_gt.NLen + 1];
        memset(padd, ' ', _gt.NLen);
        memset(&padd[_gt.NLen], 0, 1);
        printf("%s", padd);
    }
}

static void show_mesg(_gt_long flag, _gt_strg frmt, ...) {
    show_padd();
    _gt_long newl = flag & 1;
    _gt_long fore = (flag & 255) >> 1;
    char buff[30];
    snprintf(buff, sizeof(buff), "\x1b[%ldm", fore);
    fputs(buff, stdout);
    va_list argv;
    va_start(argv, frmt);
    vprintf(frmt, argv);
    va_end(argv);
    _gt_strg endl[] = {
        "\x1b[0m",
        "\x1b[0m\n"
    };
    fputs(endl[newl], stdout);
}

static void show_msex(_gt_long flag, _gt_strg frmt, ...) {
    show_padd();
    _gt_long newl = flag & 1;
    _gt_long fore = (flag & 255) >> 1;
    _gt_long back = (flag >> 9) & 127;
    char buff[30];
    snprintf(buff, sizeof(buff), "\x1b[%ld;%ldm", fore, back);
    fputs(buff, stdout);
    va_list argv;
    va_start(argv, frmt);
    vprintf(frmt, argv);
    va_end(argv);
    _gt_strg endl[] = {
        "\x1b[0m",
        "\x1b[0m\n"
    };
    fputs(endl[newl], stdout);
}

static void zero_memr(_gt_pntr strt, _gt_pntr _end) {
    _gt_long diff = _end - strt;
    memset(strt, 0, diff);
}

static void enbl_padd(void) {
    _gt.PdSt = 1;
}

static void dsbl_padd(void) {
    _gt.PdSt = 0;
}

static int init_Mdul(_gt_strg Mdul) {
    zero_memr(&_gt.Func, (&_gt.Ferr) + 1);
    _gt.Mdul = Mdul;
    return 0;
}

static int init_Func(_gt_strg Func) {
    zero_memr(&_gt.Func, &_gt.Mdul);
    _gt.Func = Func;
    _gt.NLen = strlen(Func);
    show_mesg(mesg_flag(93, 1), "%s", Func);
    return 0;
}

static int init_Case(_gt_strg Case) {
    zero_memr(&_gt.Case, &_gt.Func);
    _gt.Case = Case;
    return 0;
}

static int stop_Case(void) {
    if(!_gt.Rslt) {
        _gt_strg optr[] = {
            "==",
            "!=",
            ">",
            "<",
            ">=",
            "<="
        };
        enbl_padd();
        show_mesg(mesg_flag(93, 0), " %s ", _gt.Case);
        dsbl_padd();
        show_msex(msex_flag(97, 101, 1), "failed");
        enbl_padd();
        show_mesg(mesg_flag(94, 1), " Test: %s", _gt.Func);
        show_mesg(mesg_flag(94, 1), " File: %s (%ld)", _gt.File, _gt.Line);
        show_mesg(mesg_flag(94, 1), " Expr: %s %s %s", _gt.Exp1, optr[_gt.Cond], _gt.Exp2);
        show_mesg(mesg_flag(94, 1), " Vals: %s %s %s", _gt.Str1, optr[_gt.Cond], _gt.Str2);
        putc('\n', stdout);
        dsbl_padd();
        _gt.Fail++;
        return 1;
    }

    enbl_padd();
    show_mesg(mesg_flag(93, 0), " %s ", _gt.Case);
    dsbl_padd();
    show_msex(msex_flag(30, 102, 1), "passed");

    _gt.Pass++;
    return 1;
}

static int stop_Func(void) {
    show_mesg(mesg_flag(93, 0), "%s ", _gt.Func);
    if(_gt.Fail && _gt.Pass == 0) {
        show_msex(msex_flag(97, 101, 1), "failed");
        show_mesg(mesg_flag(94, 1), "%ld case failed.", _gt.Fail);
        _gt.Ferr++;
        return 1;
    }
    if(_gt.Fail && _gt.Pass) {
        show_msex(msex_flag(30, 103, 1), "partially failed");
        show_mesg(mesg_flag(94, 1), "%ld cases failed, %ld cases passed.", _gt.Fail, _gt.Pass);
        _gt.Ferr++;
        _gt.F_ok++;
        return 1;
    }

    show_msex(msex_flag(30, 102, 1), "passed");
    _gt.F_ok++;
    return 1;
}

static int stop_Mdul(void) {
    if(_gt.Fail) {
        show_mesg(mesg_flag(91, 1), "[%s] Module failed !", _gt.Mdul);
        show_mesg(mesg_flag(91, 0), "\t%ld failed, ", _gt.Ferr);
        show_mesg(mesg_flag(91, 1), "%ld passed.", _gt.F_ok);
        return 1;
    }

    show_mesg(mesg_flag(92, 1), "[%s] Module passed !", _gt.Mdul);
    return 1;
}

static void asrt_lctn(_gt_strg file, _gt_long line) {
    _gt.File = file;
    _gt.Line = line;
}

static int cmpr_poly(_gt_poly ply1, _gt_poly ply2, _gt_type type, _gt_cond cond) {
    int strn_rslt = (type == _gt_strg_type) ? strcmp(ply1._strg, ply2._strg) : 0;
    int rslt[_gt_cond_nmbr][_gt_type_nmbr] = {
        {
            ply1._long == ply2._long,
            ply1._dble == ply2._dble,
            ply1._pntr == ply2._pntr,
            strn_rslt == 0
        },
        {
            ply1._long != ply2._long,
            ply1._dble != ply2._dble,
            ply1._pntr != ply2._pntr,
            strn_rslt != 0
        },
        {
            ply1._long > ply2._long,
            ply1._dble > ply2._dble,
            ply1._long > ply2._long,
            strn_rslt > 0
        },
        {
            ply1._long < ply2._long,
            ply1._dble < ply2._dble,
            ply1._long < ply2._long,
            strn_rslt < 0
        },
        {
            ply1._long >= ply2._long,
            ply1._dble >= ply2._dble,
            ply1._long >= ply2._long,
            strn_rslt >= 0
        },
        {
            ply1._long <= ply2._long,
            ply1._dble <= ply2._dble,
            ply1._long <= ply2._long,
            strn_rslt <= 0
        },
    };

    return rslt[cond][type];
}

static void cvrt_strg(_gt_type type, _gt_cond cond, _gt_poly ply1, _gt_poly ply2, _gt_strg exp1, _gt_strg exp2) {
    _gt.Cond = cond;
    _gt.Exp1 = exp1;
    _gt.Exp2 = exp2;
    int len1 = 0, len2 = 0;

    switch(type) {
    case _gt_long_type:
        snprintf(_gt.Val1, sizeof(_gt.Val1), "%ld", ply1._long);
        snprintf(_gt.Val2, sizeof(_gt.Val2), "%ld", ply2._long);
        _gt.Str1 = _gt.Val1;
        _gt.Str2 = _gt.Val2;
        break;
    case _gt_dble_type:
        snprintf(_gt.Val1, sizeof(_gt.Val1), "%lf", ply1._dble);
        snprintf(_gt.Val2, sizeof(_gt.Val2), "%lf", ply2._dble);
        _gt.Str1 = _gt.Val1;
        _gt.Str2 = _gt.Val2;
        break;
    case _gt_pntr_type:
        snprintf(_gt.Val1, sizeof(_gt.Val1), "%p", ply1._pntr);
        snprintf(_gt.Val2, sizeof(_gt.Val2), "%p", ply2._pntr);
        _gt.Str1 = _gt.Val1;
        _gt.Str2 = _gt.Val2;
        break;
    case _gt_strg_type:
        _gt.Str1 = ply1._strg;
        _gt.Str2 = ply2._strg;
        break;
    case _gt_type_nmbr:
        // should not happen.
        break;
    }
}

int _gt_set_func(_gt_strg Func) { return init_Func(Func); }
int _gt_set_case(_gt_strg Case) { return init_Case(Case); }
int _gt_end_case(void) { return stop_Case(); }
int _gt_end_func(void) { return stop_Func(); }
void _gt_set_lctn(_gt_strg file, _gt_long line) { asrt_lctn(file, line); }

int _gt_chk_long(_gt_long val1, _gt_strg exp1, _gt_long val2, _gt_strg exp2, _gt_cond cond) {
    _gt_type type = _gt_long_type;
    _gt_poly ply1 = (_gt_poly){._long = val1};
    _gt_poly ply2 = (_gt_poly){._long = val2};
    _gt_long rslt = cmpr_poly(ply1, ply2, type, cond);

    if(!(_gt.Rslt = rslt)) {
        cvrt_strg(type, cond, ply1, ply2, exp1, exp2);
    }

    return _gt.Rslt;
}

int _gt_chk_dble(_gt_dble val1, _gt_strg exp1, _gt_dble val2, _gt_strg exp2, _gt_cond cond) {
    _gt_type type = _gt_dble_type;
    _gt_poly ply1 = (_gt_poly){._dble = val1};
    _gt_poly ply2 = (_gt_poly){._dble = val2};
    _gt_long rslt = cmpr_poly(ply1, ply2, type, cond);

    if(!(_gt.Rslt = rslt)) {
        cvrt_strg(type, cond, ply1, ply2, exp1, exp2);
    }

    return _gt.Rslt;
}

int _gt_chk_pntr(_gt_pntr val1, _gt_strg exp1, _gt_pntr val2, _gt_strg exp2, _gt_cond cond) {
    _gt_type type = _gt_pntr_type;
    _gt_poly ply1 = (_gt_poly){._pntr = val1};
    _gt_poly ply2 = (_gt_poly){._pntr = val2};
    _gt_long rslt = cmpr_poly(ply1, ply2, type, cond);

    if(!(_gt.Rslt = rslt)) {
        cvrt_strg(type, cond, ply1, ply2, exp1, exp2);
    }

    return _gt.Rslt;
}

int _gt_chk_strg(_gt_strg val1, _gt_strg exp1, _gt_strg val2, _gt_strg exp2, _gt_cond cond) {
    _gt_type type = _gt_strg_type;
    _gt_poly ply1 = (_gt_poly){._strg = val1};
    _gt_poly ply2 = (_gt_poly){._strg = val2};
    _gt_long rslt = cmpr_poly(ply1, ply2, type, cond);

    if(!(_gt.Rslt = rslt)) {
        cvrt_strg(type, cond, ply1, ply2, exp1, exp2);
    }

    return _gt.Rslt;
}
