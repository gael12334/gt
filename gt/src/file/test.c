/*
 * Copyright (c) 2025 Gaël Fortier <gael.fortier.1@ens.etsmtl.ca>
 */

#include "file.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

void test_gt_file_load(void)
{
    // arrange
    const char text[] = "hello";
    FILE*      file = fopen("file.txt", "w");

    assert(file != NULL);
    fwrite(text, 1, sizeof(text), file);
    fclose(file);

    // act
    gt_buf buf = { 0 };
    int    r = gt_file_load("file.txt", &buf);

    // assert
    if (r)
        printf("result was %i\n", r);
    else if (strncmp(text, buf.data, sizeof(text)) != 0)
        printf("file text not equal to %s\n", text);
    else
        printf("ok\n");

    // clean up
    if (buf.data)
        free(buf.data);

    remove("file.txt");
}

void test_gt_file_unload(void)
{
    // arrange
    gt_buf buf = { 0 };
    buf.data = malloc(10);
    assert(buf.data != NULL);
    buf.size = 10;

    // act
    int r = gt_file_unload(&buf);

    // assert
    if (r)
        printf("result was %i\n", r);
    else if (buf.data != NULL)
        printf("data other than null\n");
    else
        printf("ok\n");

    // clean up
    if (buf.data)
        free(buf.data);
}

void test_gt_file_save(void)
{
    // arrange
    char   data[] = "123456";
    gt_buf buf = { .data = data, .size = sizeof(data) };

    // act
    int r = gt_file_save(&buf, "test.txt");

    // assert
    FILE* f = fopen("test.txt", "r");
    if (f == NULL) {
        printf("test.txt absent\n");
        return;
    }

    fseek(f, 0, SEEK_END);
    int size = ftell(f);
    if (size != sizeof(data))
        printf("size is different\n");
    else
        printf("ok\n");

    // clean up
    fclose(f);
    remove("test.txt");
}

void test_gt_file_exists(void)
{
    // arrange

    // act
    int exists = 0;
    int r = gt_file_exists("test.txt", &exists);

    // assert
    if (r)
        printf("result was %i\n", r);
    else if (exists != 0)
        printf("exists was false\n");
    else
        printf("ok\n");
}

int main(int argc, char** argv)
{
    test_gt_file_load();
    test_gt_file_unload();
    test_gt_file_save();
    test_gt_file_exists();
    return 0;
}
