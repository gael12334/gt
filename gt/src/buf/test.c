#include "buf.h"
#include <stdio.h>

void test_gt_buf_init(void)
{
    // arrange
    char   data[64] = { 0 };
    gt_buf buf = { 0 };

    // act
    int result = gt_buf_init(sizeof(data), data, &buf);

    // assert
    if (result != GT_BUF_OK)
        printf("result was %i\n", result);
    else
        printf("ok\n");
}

void test_gt_buf_deinit(void)
{
    // arrange
    char   data[64] = { 0 };
    gt_buf buf = { .data = data, .size = sizeof(data) };

    // act
    int result = gt_buf_deinit(&buf);

    // assert
    if (result != GT_BUF_OK)
        printf("result was %i\n", result);
    else
        printf("ok\n");
}

void test_gt_buf_segment(void)
{
    // arrange
    char   data[64] = { 0 };
    gt_buf buf = { .data = data, .size = sizeof(data) };
    size_t offset = 10;
    size_t size = 10;
    gt_buf seg = { 0 };

    // act
    int result = gt_buf_segment(&buf, offset, size, &seg);

    // assert
    if (result != GT_BUF_OK)
        printf("result was %i\n", result);
    else if (seg.data != buf.data + offset)
        printf("seg.data = %p; buf.data = %p\n", seg.data, buf.data + offset);
    else
        printf("ok\n");
}

void test_gt_buf_write(void)
{
    // arrange
    char     data[64] = { 0 };
    gt_buf   buf = { .data = data, .size = sizeof(data) };
    uint32_t value = 0x66666666;

    // act
    int result = gt_buf_write(&buf, sizeof(value), &value);

    // assert
    if (result != GT_BUF_OK)
        printf("result was %i\n", result);
    else if (data[3] != 0x66)
        printf("data at %p was %hhi\n", &data[3], data[3]);
    else
        printf("ok\n");
}

void test_gt_buf_read(void)
{
    // arrange
    char     data[64] = { 0 };
    gt_buf   buf = { .data = data, .size = sizeof(data) };
    gt_buf   seg = { .data = buf.data, .size = sizeof(uint32_t) };
    uint32_t value = 0x66666666;

    memcpy(data, &value, sizeof(value));
    value = 0;

    // act
    int result = gt_buf_read(&seg, sizeof(value), &value);

    // assert
    if (result != GT_BUF_OK)
        printf("result was %i\n", result);
    else if (value != 0x66666666)
        printf("value was %i\n", value);
    else
        printf("ok\n");
}

void test_gt_buf_distance(void)
{
    // arrange
    char     data[64] = { 0 };
    uint32_t offset = 10;
    gt_buf   buf = { .data = data, .size = sizeof(data) };
    gt_buf   seg = { .data = buf.data + offset, .size = 1 };

    // act
    size_t distance = 0;
    int    result = gt_buf_distance(&buf, &seg, &distance);

    // assert
    if (result != GT_BUF_OK)
        printf("result was %i\n", result);
    else if (distance != offset)
        printf("distance was %zu", distance);
    else
        printf("ok\n");
}

void test_gt_buf_data(void)
{
    // arrange
    char   data[64] = { 0 };
    gt_buf buf = { .data = data, .size = sizeof(data) };

    // act
    void* ptr = NULL;
    int   result = gt_buf_data(&buf, &ptr);

    // assert
    if (result != GT_BUF_OK)
        printf("result was %i\n", result);
    else if (ptr != data)
        printf("data was %p", ptr);
    else
        printf("ok\n");
}

void test_gt_buf_size(void)
{
    // arrange
    char   data[64] = { 0 };
    gt_buf buf = { .data = data, .size = sizeof(data) };

    // act
    size_t size = 0;
    int    result = gt_buf_size(&buf, &size);

    // assert
    if (result != GT_BUF_OK)
        printf("result was %i\n", result);
    else if (size != buf.size)
        printf("size was %zu", size);
    else
        printf("ok\n");
}

void test_gt_buf_equals(void)
{
    // arrange
    char   data1[] = "hello world!!!";
    gt_buf buf1 = { .data = data1, .size = sizeof(data1) };

    char   data2[] = "hello world!!!";
    gt_buf buf2 = { .data = data2, .size = sizeof(data2) };

    // act
    int equals = 0;
    int result = gt_buf_equals(&buf1, &buf2, &equals);

    // assert
    if (result != GT_BUF_OK)
        printf("result was %i\n", result);
    else if (equals == 0)
        printf("equals was %i\n", equals);
    else
        printf("ok\n");
}

int main(int argc, char** argv)
{
    test_gt_buf_init();
    test_gt_buf_deinit();
    test_gt_buf_segment();
    test_gt_buf_write();
    test_gt_buf_read();
    test_gt_buf_distance();
    test_gt_buf_data();
    test_gt_buf_size();
    test_gt_buf_equals();
    return 0;
}
