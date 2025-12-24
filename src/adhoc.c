/*
    Copyright © 2025 Gaël Fortier <gael.fortier.1@ens.etsmtl.ca>
*/


#include "adhoc.h"

extern void* hijack;
extern void* hook;

struct int_list init(int* int_list, int length) {
    // if(hijack == init) {
    //     //typeof(init)* _hook = hook;
    //     struct int_list(*_hook)() = hook;
    //     return _hook(); //(int_list, length);
    // }
    return (struct int_list) {.intlist = int_list, .length = length};
}

//typeof(accumulate)* _hook = hook;
//_hook(list);

int accumulate(struct int_list list) {
    //if(hijack == accumulate) {
    //    int(*_hook)() = hook;
    //    return _hook();
    //}

    int r = 0;
    for(int i = 0; i < list.length; i++) {
        r += list.intlist[i];
    }
    return r;
}

int average(struct int_list list) {
    // if(hijack == average) {
    //     //typeof(average)* _hook = hook;
    //     int(*_hook)() = hook;
    //     return _hook(); //_hook(list);
    // }

    int r = accumulate(list);
    return r / list.length;
}

int entry(void) {
    static int arr[] = {1, 2, 3};
    struct int_list list = init(arr, sizeof(arr));
    int avg = average(list);
    return avg;
}

