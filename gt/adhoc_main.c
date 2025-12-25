/*
    Copyright © 2025 Gaël Fortier <gael.fortier.1@ens.etsmtl.ca>
*/

#include <stdio.h>
#include "adhoc.h"

void* hijack;
void* hook;

int hook_func(struct int_list list) {
    printf("length in hook_func: %i\n", list.length);
    printf("int_list in hook_func: %p\n", list.intlist);
    return 10000;
}

int main() {
    hijack = accumulate;
    hook = hook_func;

    int r = entry();
    printf("%i\n", r);

    return 0;
}
