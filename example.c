#include <stdio.h>
#include <string.h>

#include "ringbuf.h"

/**
 * @brief 定义16字节的环形缓冲区
 * @note 缓冲区大小必须是2的倍数
 */
RINGBUF_DEFINE(mybuf, 16);

int main(void)
{
    char input_data[] = "hello";
    char output_data[16];
    size_t written, read;

    /* 写入数据到缓冲区 */
    written = ringbuf_put(&mybuf, input_data, strlen(input_data));
    printf("Put %zu bytes to ring buffer.\n", written);

    /* 从缓冲区读取数据 */
    memset(output_data, 0, sizeof(output_data));
    read = ringbuf_get(&mybuf, output_data, sizeof(output_data));
    printf("Got %zu bytes from ring buffer: '%s'\n", read, output_data);

    return 0;
}
