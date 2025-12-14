/*
    Copyright © 2025 Gaël Fortier <gael.fortier.1@ens.etsmtl.ca>
*/

#include "calc.h"
#include "calc_test.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

float* get_values(int argc, char** argv, unsigned* out_size) {
    float* set = malloc(sizeof(float) * (argc - 2));
    unsigned index = 0;
    
    for(char** argv_it = argv + 2; argv_it != argv + argc; argv_it++) {
        set[index++] = atof(*argv_it);
    }

    *out_size = index;
    return set;
}

void show_result(int argc, char** argv, int choice) {
    typedef float(*calc_fn)(const float*, unsigned);
    calc_fn functions[3] = {calc_mean, calc_median, calc_sum};

    unsigned size;
    float* set = get_values(argc, argv, &size);
    float result = functions[choice - 1](set, size);

    printf("Result: %f\n", result);
    free(set);
} 

int main(int argc, char** argv) {
    if(argc >= 2 && strcmp(argv[1], "test") == 0) {
        calc_test();
        return 0;
    }

    if(argc >= 2 && strcmp(argv[1], "mean") == 0) {
        show_result(argc, argv, 1);
        return 0;
    }

    if(argc >= 2 && strcmp(argv[1], "median") == 0) {
        show_result(argc, argv, 2);
        return 0;
    }

    if(argc >= 2 && strcmp(argv[1], "sum") == 0) {
        show_result(argc, argv, 3);
        return 0;
    }

    printf("Invalid arguments.\n");
    return 0;
}