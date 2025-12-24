/*
    Copyright © 2025 Gaël Fortier <gael.fortier.1@ens.etsmtl.ca>
*/

#pragma once

struct int_list {
    int* intlist;
    int length;
};

struct int_list init(int* int_list, int length);

int accumulate(struct int_list list);

int average(struct int_list list);

int entry(void);
