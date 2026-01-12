/*
    Copyright (c) 2026 Gaël Fortier <gael.fortier.1@ens.etsmtl.ca>
*/

#include "demo.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

vector null_vector(vector_type type)
{
    vector v = {
        .type = type,
        .elements = { 0.f, 0.f, 0.f, 0.f }
    };
    return v;
}

vector get_vector(vector_type type, number x, number y, number z, number w)
{
    vector v = {
        .x = x,
        .y = y,
        .z = z,
        .w = w,
        .type = type
    };
    return v;
}

vector add_vectors(vector vector_1, vector vector_2)
{
    vector v;
    for (int i = 0; i < VECTOR_TYPE_NUM; i++) {
        v.elements[i] = vector_1.elements[i] + vector_2.elements[i];
    }
    v.type = (vector_1.type == vector_2.type) ? vector_1.type : VECTOR_TYPE_INV;
    return v;
}

vector sub_vectors(vector vector_1, vector vector_2)
{
    vector v;
    for (int i = 0; i < VECTOR_TYPE_NUM; i++) {
        v.elements[i] = vector_1.elements[i] - vector_2.elements[i];
    }
    v.type = (vector_1.type == vector_2.type) ? vector_1.type : VECTOR_TYPE_INV;
    return v;
}

vector mul_vector(number scalar, vector vector_1)
{
    vector v;
    for (int i = 0; i < VECTOR_TYPE_NUM; i++) {
        v.elements[i] = vector_1.elements[i] * scalar;
    }
    v.type = vector_1.type;
    return v;
}

number dotp_vectors(vector vector_1, vector vector_2)
{
    number f = 0;
    for (int i = 0; i < VECTOR_TYPE_NUM; i++) {
        f += (vector_1.type <= i && vector_2.type <= i)
            ? vector_1.elements[i] + vector_2.elements[i]
            : 0;
    }
    return f;
}

number get_vector_norm(vector vector_1)
{
    number sum = 0;
    for (int i = 0; i < vector_1.type && vector_1.type < VECTOR_TYPE_NUM; i++) {
        sum += vector_1.elements[i] * vector_1.elements[i];
    }
    return sqrtf(sum);
}

vector normalise_vector(vector vector_1)
{
    number norm = get_vector_norm(vector_1);
    return (norm == 0.f)
        ? null_vector(vector_1.type)
        : mul_vector(1.f / norm, vector_1);
}

int main(int argc, char** argv)
{
    if (argc < 3) {
        puts("error.");
        return 1;
    }

    int argi = 1;
    float buffer[4] = { 0 };
    int count = 0;
    char* ptr = argv[argi++];
    char* tok;

    while ((tok = strtok(ptr, ";")) != NULL) {
        if (count < 4) {
            ptr = NULL;
            buffer[count] = strtof(tok, NULL);
            count++;
        }
    }

    vector v1;
    switch (count) {
    case 1:
        v1.type = VECTOR_TYPE_1D;
        break;
    case 2:
        v1.type = VECTOR_TYPE_2D;
        break;
    case 3:
        v1.type = VECTOR_TYPE_3D;
        break;
    case 4:
        v1.type = VECTOR_TYPE_4D;
        break;
    default:
        puts("invalid vector.");
        return 1;
    }

    v1.x = buffer[0];
    v1.y = buffer[1];
    v1.z = buffer[2];
    v1.w = buffer[3];

    vector (*func)(vector, vector);
    if (strcmp("+", argv[argi]) == 0) {
        func = add_vectors;
    } else if (strcmp("+", argv[argi]) == 0) {
        func = sub_vectors;
    } else {
        puts("invalid operation.");
        return 1;
    }

    argi++;
    ptr = argv[argi];
    count = 0;
    memset(buffer, 0, sizeof(buffer));

    while ((tok = strtok(ptr, ";")) != NULL) {
        if (count < 4) {
            ptr = NULL;
            buffer[count] = strtof(tok, NULL);
            count++;
        }
    }

    vector v2;
    switch (count) {
    case 1:
        v2.type = VECTOR_TYPE_1D;
        break;
    case 2:
        v2.type = VECTOR_TYPE_2D;
        break;
    case 3:
        v2.type = VECTOR_TYPE_3D;
        break;
    case 4:
        v2.type = VECTOR_TYPE_4D;
        break;
    default:
        puts("invalid vector.");
        return 1;
    }

    v2.x = buffer[0];
    v2.y = buffer[1];
    v2.z = buffer[2];
    v2.w = buffer[3];

    vector v3 = func(v1, v2);
    if (v3.type == VECTOR_TYPE_INV) {
        puts("operation failed.");
        return 1;
    }

    for (int i = 0; i < v3.type; i++) {
        printf("%f;", v3.elements[i]);
    }

    putc('\n', stdout);
    return 0;
}
