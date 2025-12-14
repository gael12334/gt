/*
    Copyright © 2025 Gaël Fortier <gael.fortier.1@ens.etsmtl.ca>
*/

#include "calc.h"

int calc_even(int value) {
    return value % 2 == 0;
}

float calc_median(const float* set, unsigned size) {
    if(size <= 0 || set == 0x0) {
        return 0.f;
    }

    int is_even = calc_even(size);
    int low = (size / 2) - is_even;
    int high = low + is_even;
    float median = (set[low] + set[high]) / 2.f;
    return median;
}

float calc_mean(const float* set, unsigned size) {
    if(size <= 0 || set == 0x0) {
        return 0.f;
    }

    float sum = calc_sum(set, size);
    sum /= ((float)size);
    return sum;
}

float calc_sum(const float* set, unsigned size) {
    float sum = 0.f;
    for(const float* it = set; it != set + size && it != 0x0; it++) {
        sum += *it;
    }
    return sum;
}
