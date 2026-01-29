/*
    Copyright (c) 2026 Gaël Fortier <gael.fortier.1@ens.etsmtl.ca>
*/

#pragma once

typedef float number;

typedef enum vector_type {
    VECTOR_TYPE_1D,
    VECTOR_TYPE_2D,
    VECTOR_TYPE_3D,
    VECTOR_TYPE_4D,
    VECTOR_TYPE_NUM,
    VECTOR_TYPE_INV,
} vector_type;

typedef struct vector {
    union {
        struct {
            number x, y, z, w;
        };
        number elements[VECTOR_TYPE_NUM];
    };
    vector_type type;
} vector;

vector null_vector(vector_type type);
vector get_vector(vector_type type, number x, number y, number z, number w);
vector add_vectors(vector vector_1, vector vector_2);
vector sub_vectors(vector vector_1, vector vector_2);
vector mul_vector(number scalar, vector vector_1);
number dotp_vectors(vector vector_1, vector vector_2);
number get_vector_norm(vector vector_1);
vector normalise_vector(vector vector_1);
int main(int argc, char** argv);
