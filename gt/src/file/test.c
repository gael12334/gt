/*
 * Copyright (c) 2025 Gaël Fortier <gael.fortier.1@ens.etsmtl.ca>
 */

#include "file.h"

int main(int argc, char** argv)
{
    const char content[] = "hello";

    int test_gt_file_load = 0;
    while (!test_gt_file_load) {
        // arrange
        FILE* fd = fopen("./test.txt", "w");
        if (fd == NULL) {
            printf("failed to write test.txt\n");
            break;
        }

        fwrite(content, 1, sizeof(content), fd);
        fclose(fd);

        // act
        gt_file file = { 0 };
        int     error = gt_file_load("./test.txt", &file);

        // assert
        if (error) {
            printf("error %i\n", error);
            break;
        }

        if (file.size != sizeof(content)) {
            printf("size is %lu, expected %lu\n", file.size, sizeof(content));
            break;
        }

        if (memcmp(content, file.bytes, file.size) != 0) {
            printf("data from file does not match\n");
            break;
        }

        free(file.bytes);
        remove(file.path);
        printf("ok\n");
        test_gt_file_load = 1;
    }

    int test_gt_file_unload = 0;
    while (!test_gt_file_unload) {
        // arrange
        uint8_t* buf = malloc(sizeof(content));
        memcpy(buf, content, sizeof(content));
        gt_file file = { .bytes = buf, .size = sizeof(content), .path = {} };

        // act
        int error = gt_file_unload(&file);

        // assert
        if (error) {
            printf("error %i\n", error);
            free(buf);
            break;
        }

        printf("ok\n");
        test_gt_file_unload = 1;
    }

    int test_gt_file_save = 0;
    while (!test_gt_file_save) {
        // arrange
        uint8_t* buf = malloc(sizeof(content));
        memcpy(buf, content, sizeof(content));
        gt_file file = { .bytes = buf, .size = sizeof(content), .path = {} };

        // act
        int error = gt_file_save("./test.txt", &file);

        // assert
        if (error) {
            printf("error %i\n", error);
            free(buf);
            break;
        }

        FILE* fd = fopen("./test.txt", "r");
        if (fd == NULL) {
            printf("file was not saved\n");
            free(buf);
            break;
        }

        printf("ok\n");
        fclose(fd);
        remove("./test.txt");
        free(buf);
        test_gt_file_save = 1;
    }

    int test_gt_file_getaddr = 0;
    while (!test_gt_file_getaddr) {
        // arrange
        gt_file file = { .bytes = (uint8_t*)content, .size = sizeof(content), .path = {} };
        int     index = 2;

        // act
        uint8_t* result;
        int      error = gt_file_getaddr(&file, index, (void**)&result);

        // assert
        if (error) {
            printf("error %i\n", error);
            break;
        }

        if (*result != content[index]) {
            printf("byte at index %i (%hhu) was not expected\n", index, *result);
            break;
        }

        printf("ok\n");
        test_gt_file_getaddr = 1;
    }

    int test_gt_file_writeat = 0;
    while (!test_gt_file_writeat) {
        // arrange
        uint8_t writable_buffer[64];
        gt_file file = { .bytes = writable_buffer, .size = sizeof(writable_buffer), .path = {} };
        int     index = 2;

        // act
        uint8_t* result;
        int      error = gt_file_writeat(&file, index, (void*)content, sizeof(content));

        // assert
        if (error) {
            printf("error %i\n", error);
            break;
        }

        if (strcmp((char*)&file.bytes[index], content) != 0) {
            printf("data written differs from content\n");
            break;
        }

        printf("ok\n");
        test_gt_file_writeat = 1;
    }

    int test_gt_file_getrange = 0;
    while (!test_gt_file_getrange) {
        // arrange
        gt_file file = { .bytes = (uint8_t*)content, .size = sizeof(content), .path = {} };
        int     start = 2;
        int     size = 3;

        // act
        void *startptr, *endptr;
        int   error = gt_file_getrange(&file, start, size, &startptr, &endptr);

        // assert
        if (error) {
            printf("error %i\n", error);
            break;
        }

        if (startptr != &content[start] || endptr != &content[start + size]) {
            printf("range is invalid\n");
            break;
        }

        printf("ok\n");
        test_gt_file_getrange = 1;
    }
}
