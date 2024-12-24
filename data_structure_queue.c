#include <stdio.h>
#include <string.h>

#include "ringbuf.h"

// 定义16字节的队列缓冲区
RINGBUF_DEFINE(queue_buf, 16);

/**
 * @brief 入队操作
 * @param data 要写入的数据
 * @param length 数据长度
 * @return 成功写入的字节数
 */
static size_t queue_enqueue(const void *data, size_t length)
{
    return ringbuf_put(&queue_buf, data, length);
}

/**
 * @brief 出队操作
 * @param buffer 接收数据的缓冲区
 * @param size 缓冲区大小
 * @return 实际读取的字节数
 */
static size_t queue_dequeue(void *buffer, size_t size)
{
    return ringbuf_get(&queue_buf, buffer, size);
}

/**
 * @brief 查看队首元素（不移除）
 * @param buffer 接收数据的缓冲区
 * @param size 缓冲区大小
 * @return 实际读取的字节数
 */
static size_t queue_peek(void *buffer, size_t size)
{
    return ringbuf_peek(&queue_buf, buffer, size);
}

/**
 * @brief 获取队列当前长度
 * @return 队列中的字节数
 */
static size_t queue_length(void)
{
    return ringbuf_length(&queue_buf);
}

int main(void)
{
    printf("=== 队列(Queue/FIFO)测试 ===\n\n");

    // 测试初始化状态
    printf("1. 测试初始化状态\n");
    printf("初始队列长度: %zu\n", queue_length());
    if (queue_length() != 0) {
        printf("❌ 初始化测试失败：队列长度不为0\n");
        return 1;
    }
    printf("✓ 初始化测试通过\n\n");

    // 测试入队操作
    printf("2. 测试入队操作\n");
    const char *data1 = "Hello";
    size_t written = queue_enqueue(data1, strlen(data1));
    printf("入队 %zu 字节: '%s'\n", written, data1);
    if (written != strlen(data1)) {
        printf("❌ 入队测试失败：写入长度不匹配\n");
        return 1;
    }

    const char *data2 = "World";
    written = queue_enqueue(data2, strlen(data2));
    printf("入队 %zu 字节: '%s'\n", written, data2);
    if (written != strlen(data2)) {
        printf("❌ 入队测试失败：写入长度不匹配\n");
        return 1;
    }

    printf("当前队列长度: %zu\n", queue_length());
    if (queue_length() != strlen(data1) + strlen(data2)) {
        printf("❌ 入队测试失败：队列长度不正确\n");
        return 1;
    }
    printf("✓ 入队测试通过\n\n");

    // 测试查看队首
    printf("3. 测试查看队首\n");
    char peek_buf[16] = {0};
    size_t peeked = queue_peek(peek_buf, sizeof(peek_buf));
    printf("查看队首 %zu 字节: '%s'\n", peeked, peek_buf);
    if (peeked != strlen(data1) + strlen(data2) ||
        strncmp(peek_buf, "HelloWorld", peeked) != 0) {
        printf("❌ 查看队首测试失败：数据不匹配\n");
        return 1;
    }
    printf("✓ 查看队首测试通过\n\n");

    // 测试出队操作
    printf("4. 测试出队操作\n");
    char buf[16] = {0};
    size_t read = queue_dequeue(buf, sizeof(buf));
    printf("出队 %zu 字节: '%s'\n", read, buf);
    if (read != strlen(data1) + strlen(data2) ||
        strncmp(buf, "HelloWorld", read) != 0) {
        printf("❌ 出队测试失败：数据不匹配\n");
        return 1;
    }

    // 测试空队列操作
    printf("\n5. 测试空队列操作\n");
    memset(buf, 0, sizeof(buf));
    read = queue_dequeue(buf, sizeof(buf));
    printf("从空队列出队 %zu 字节\n", read);
    if (read != 0) {
        printf("❌ 空队列测试失败：应返回0字节\n");
        return 1;
    }

    // 测试队列已满情况
    printf("\n6. 测试队列已满情况\n");
    char full_data[20] = "0123456789ABCDEF01";  // 17字节，超过缓冲区大小
    written = queue_enqueue(full_data, strlen(full_data));
    printf("尝试写入超过缓冲区大小的数据: %zu 字节\n", written);
    if (written >= sizeof(full_data)) {
        printf("❌ 队列已满测试失败：不应写入超过缓冲区大小的数据\n");
        return 1;
    }

    printf("\n最终队列长度: %zu\n", queue_length());
    printf("✓ 所有队列测试通过！\n");
    return 0;
}
