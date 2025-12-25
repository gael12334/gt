/*
    Copyright © 2025 Gaël Fortier <gael.fortier.1@ens.etsmtl.ca>
*/

#include "demo.h"

struct int_list init(int* int_list, int length) {
    return (struct int_list) {.intlist = int_list, .length = length};
}

int accumulate(struct int_list list) {
    int r = 0;
    for(int i = 0; i < list.length; i++) {
        r += list.intlist[i];
    }
    return r;
}

int average(struct int_list list) {
    int r = accumulate(list);
    return r / list.length;
}

int entry(void) {
    static int arr[] = {1, 2, 3};
    struct int_list list = init(arr, sizeof(arr));
    int avg = average(list);
    return avg;
}
