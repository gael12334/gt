/*
    Copyright (c) 2026 Gaël Fortier <gael.fortier.1@ens.etsmtl.ca>
*/

#include "gt2.h"
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

static const char* const gt_op[] = {
    "==",
    "!=",
    "<",
    ">",
    "<=",
    ">="
};

static struct {
    struct {
        const char* name;
        uint64_t success_nb;
        uint64_t failure_nb;
    } Function;

    struct {
        const char* name;
        uint64_t result;
    } Case;

    struct {
        gt_mode mode;
        gt_expression A;
        gt_expression B;
        const char* file;
        unsigned int line;
    } Assert;
} global = { 0 };

#define gt_symbol_name(x) #x
void* gt_hijack = NULL;
void* gt_mock = NULL;
void* gt_data = NULL;

static void gt_set_output_color(int reset, int foreground, int background)
{
    if ((foreground >= 30 && foreground <= 37) || (foreground > 90 && foreground < 97)) {
        printf("\x1b[%im", foreground);
    }

    if ((background >= 40 && background <= 47) || (background > 100 && background < 107)) {
        printf("\x1b[%im", background);
    }

    if (reset) {
        printf("\x1b[0m");
    }
}

static void gt_print(int foreground, int background, const char* format, ...)
{
    va_list argv;
    va_start(argv, format);
    gt_set_output_color(0, foreground, background);
    vprintf(format, argv);
    gt_set_output_color(1, 0, 0);
    va_end(argv);
}

static void gt_println(int foreground, int background, const char* format, ...)
{
    va_list argv;
    va_start(argv, format);
    gt_set_output_color(0, foreground, background);
    vprintf(format, argv);
    gt_set_output_color(1, 0, 0);
    putc('\n', stdout);
    va_end(argv);
}

static void gt_print_indent(void)
{
    static const char indent[] = { ' ', ' ', ' ', ' ' };
    fwrite(indent, 1, sizeof(indent), stdout);
}

static void gt_print_generic(gt_generic generic, gt_type type, int fore, int back, void (*print)(int, int, const char*, ...))
{
    switch (type) {
    case GT_TYPE_U_64:
        print(fore, back, "%lu", generic.u_64);
        break;
    case GT_TYPE_S_64:
        print(fore, back, "%li", generic.s_64);
        break;
    case GT_TYPE_F_64:
        print(fore, back, "%lf", generic.f_64);
        break;
    case GT_TYPE_CSTR:
        print(fore, back, "%s", generic.cstr);
        break;
    case GT_TYPE_ADDR:
        print(fore, back, "%p", generic.addr);
        break;
    default:
        print(fore, back, "invalid type");
    }
}

gt_generic gt_explicit_genericof_u_64(uint64_t u_64)
{
    return (gt_generic) { .u_64 = u_64 };
}

gt_generic gt_explicit_genericof_s_64(int64_t s_64)
{
    return (gt_generic) { .s_64 = s_64 };
}

gt_generic gt_explicit_genericof_f_64(double f_64)
{
    return (gt_generic) { .f_64 = f_64 };
}

gt_generic gt_explicit_genericof_cstr(const char* str)
{
    return (gt_generic) { .cstr = str };
}

gt_generic gt_explicit_genericof_vptr(const void* ptr)
{
    return (gt_generic) { .addr = ptr };
}

int gt_explicit_test_function(const char* name)
{
    static int mode = GT_LOOP_LEAVE;

    if (mode == GT_LOOP_LEAVE) {
        mode = GT_LOOP_ENTER;
        global.Function.name = name;
        global.Function.failure_nb = 0;
        global.Function.success_nb = 0;
        gt_println(33, 0, "%s", name);
    } else {
        mode = GT_LOOP_LEAVE;
        gt_print_indent();
        if (global.Function.failure_nb == 0 && global.Function.success_nb > 0) {
            gt_println(36, 0, "%lu test case passed", global.Function.success_nb);
        } else if (global.Function.failure_nb > 0 && global.Function.success_nb > 0) {
            gt_println(36, 0, "%lu test case passed, %lu test case failed", global.Function.success_nb, global.Function.failure_nb);
        } else if (global.Function.failure_nb > 0 && global.Function.success_nb == 0) {
            gt_println(36, 0, "%lu test case failed", global.Function.failure_nb);
        } else {
            gt_print(33, 0, "%s ", name);
            gt_println(0, 0, " skipped");
        }
    }

    return mode;
}

int gt_explicit_test_case(const char* name)
{
    static int mode = GT_LOOP_LEAVE;
    static uint64_t length = 0;

    if (mode == GT_LOOP_LEAVE) {
        mode = GT_LOOP_ENTER;
        global.Case.name = name;
        global.Case.result = GT_LOOP_ENTER;
        length = strlen(name);
        gt_print_indent();
        gt_print(33, 0, "%s ", name);
        for (uint64_t i = 0; i < (30 - length); i++) {
            gt_print(0, 0, ".");
        }

    } else {
        mode = GT_LOOP_LEAVE;
        if (global.Case.result == GT_LOOP_ENTER) {
            gt_println(30, 102, "PASS");
            global.Function.success_nb++;
        } else {
            gt_println(30, 101, "FAIL");
            gt_print_indent();
            gt_print_indent();
            gt_println(0, 0, "file: %s", global.Assert.file);
            gt_print_indent();
            gt_print_indent();
            gt_println(0, 0, "line: %u", global.Assert.line);
            gt_print_indent();
            gt_print_indent();
            gt_println(0, 0, "expr: %s %s %s", global.Assert.A.expression, gt_op[global.Assert.mode], global.Assert.B.expression);
            gt_print_indent();
            gt_print_indent();
            gt_print(0, 0, "-> %s = ", global.Assert.A.expression);
            gt_print_generic(global.Assert.A.generic, global.Assert.A.type, 91, 0, gt_println);
            gt_print_indent();
            gt_print_indent();
            gt_print(0, 0, "-> %s = ", global.Assert.B.expression);
            gt_print_generic(global.Assert.B.generic, global.Assert.B.type, 91, 0, gt_println);
            global.Function.failure_nb++;
        }
    }

    return mode;
}

int gt_explicit_assert(gt_expression a, gt_mode mode, gt_expression b, const char* file, unsigned line)
{
    int assert_condition[GT_MODE_NUMBR] = { 0 };
    int string_comapre = 0;

    switch (a.type) {
    case GT_TYPE_ADDR:
    case GT_TYPE_U_64:
        assert_condition[GT_MODE_EQUAL] = (a.generic.u_64 == b.generic.u_64);
        assert_condition[GT_MODE_NT_EQ] = (a.generic.u_64 != b.generic.u_64);
        assert_condition[GT_MODE_LESSR] = (a.generic.u_64 < b.generic.u_64);
        assert_condition[GT_MODE_BIGGR] = (a.generic.u_64 > b.generic.u_64);
        assert_condition[GT_MODE_LE_EQ] = (a.generic.u_64 <= b.generic.u_64);
        assert_condition[GT_MODE_GR_EQ] = (a.generic.u_64 >= b.generic.u_64);
        break;
    case GT_TYPE_S_64:
        assert_condition[GT_MODE_EQUAL] = (a.generic.s_64 == b.generic.s_64);
        assert_condition[GT_MODE_NT_EQ] = (a.generic.s_64 != b.generic.s_64);
        assert_condition[GT_MODE_LESSR] = (a.generic.s_64 < b.generic.s_64);
        assert_condition[GT_MODE_BIGGR] = (a.generic.s_64 > b.generic.s_64);
        assert_condition[GT_MODE_LE_EQ] = (a.generic.s_64 <= b.generic.s_64);
        assert_condition[GT_MODE_GR_EQ] = (a.generic.s_64 >= b.generic.s_64);
        break;
    case GT_TYPE_F_64:
        assert_condition[GT_MODE_EQUAL] = (a.generic.f_64 == b.generic.f_64);
        assert_condition[GT_MODE_NT_EQ] = (a.generic.f_64 != b.generic.f_64);
        assert_condition[GT_MODE_LESSR] = (a.generic.f_64 < b.generic.f_64);
        assert_condition[GT_MODE_BIGGR] = (a.generic.f_64 > b.generic.f_64);
        assert_condition[GT_MODE_LE_EQ] = (a.generic.f_64 <= b.generic.f_64);
        assert_condition[GT_MODE_GR_EQ] = (a.generic.f_64 >= b.generic.f_64);
        break;
    case GT_TYPE_CSTR:
        string_comapre = strcmp(a.generic.cstr, b.generic.cstr);
        assert_condition[GT_MODE_EQUAL] = (string_comapre == 0);
        assert_condition[GT_MODE_NT_EQ] = (string_comapre != 0);
        assert_condition[GT_MODE_LESSR] = (string_comapre < 0);
        assert_condition[GT_MODE_BIGGR] = (string_comapre > 0);
        assert_condition[GT_MODE_LE_EQ] = (string_comapre <= 0);
        assert_condition[GT_MODE_GR_EQ] = (string_comapre >= 0);
        break;
    case GT_TYPE_NMBR:
        break;
    }

    if (assert_condition[mode] == 0) {
        global.Assert.A = a;
        global.Assert.B = b;
        global.Assert.file = file;
        global.Assert.line = line;
        global.Assert.mode = mode;
        return (global.Case.result = GT_LOOP_LEAVE);
    }

    return (global.Case.result = GT_LOOP_ENTER);
}

void gt_explicit_set_mock(void* function_ptr, void* mock_ptr, void* data)
{
    gt_hijack = function_ptr;
    gt_mock = mock_ptr;
    gt_data = data;
}

void* gt_explicit_get_data(void)
{
    return gt_data;
}

int gt_explicit_unmocked_section(void** mock)
{
    static int mode = GT_LOOP_LEAVE;

    if (mode == GT_LOOP_LEAVE) {
        mode = GT_LOOP_ENTER;
        *mock = gt_mock;
        gt_mock = NULL;
    } else {
        mode = GT_LOOP_LEAVE;
        gt_mock = *mock;
    }

    return mode;
}

const char* gt_function_hijack_symbol_name(void)
{
    return gt_symbol_name(gt_hijack);
}

const char* gt_function_mock_symbol_name(void)
{
    return gt_symbol_name(gt_mock);
}

const char* gt_function_main_symbol_name(void)
{
    return "gt_test_main";
}
