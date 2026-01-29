#include "buf.h"
#include <stdio.h>

void test_gt_buf_init(void)
{
    // arrange
    static char   data[64] = { 0 };
    static gt_buf buf = { 0 };

    // act
    int result = gt_buf_init(sizeof(data), data, &buf);

    // assert
    if (result != GT_BUF_OK)
        printf("result was %i\n", result);
    else
        printf("ok\n");
}

int main(int argc, char** argv)
{
    test_gt_buf_init();
    return 0;
}
