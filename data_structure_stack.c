#include <stdio.h>
#include <string.h>

#include "ringbuf.h"

// 定义16字节的栈缓冲区
RINGBUF_DEFINE(stack_buf, 16);

/**
 * @brief 入栈操作
 * @param data 要写入的数据
 * @param length 数据长度
 * @return 成功写入的字节数
 */
static size_t stack_push(const void *data, size_t length)
{
    return ringbuf_put(&stack_buf, data, length);
}

/**
 * @brief 出栈操作（从尾部读取）
 * @param buffer 接收数据的缓冲区
 * @param size 缓冲区大小
 * @return 实际读取的字节数
 */
static size_t stack_pop(void *buffer, size_t size)
{
    size_t length = ringbuf_length(&stack_buf);
    if (length == 0) return 0;

    // 只读取最后入栈的数据
    size_t to_read = (size < length) ? size : length;
    return ringbuf_get(&stack_buf, buffer, to_read);
}

/**
 * @brief 查看栈顶元素（不移除）
 * @param buffer 接收数据的缓冲区
 * @param size 缓冲区大小
 * @return 实际读取的字节数
 */
static size_t stack_peek(void *buffer, size_t size)
{
    return ringbuf_peek(&stack_buf, buffer, size);
}

/**
 * @brief 获取栈当前深度
 * @return 栈中的字节数
 */
static size_t stack_depth(void)
{
    return ringbuf_length(&stack_buf);
}

int main(void)
{
    printf("=== 栈(Stack/LIFO)测试 ===\n\n");

    // 测试初始化状态
    printf("1. 测试初始化状态\n");
    printf("初始栈深度: %zu\n", stack_depth());
    if (stack_depth() != 0) {
        printf("❌ 初始化测试失败：栈深度不为0\n");
        return 1;
    }
    printf("✓ 初始化测试通过\n\n");

    // 测试入栈操作
    printf("2. 测试入栈操作\n");
    const char *data1 = "First";
    size_t pushed = stack_push(data1, strlen(data1));
    printf("入栈 %zu 字节: '%s'\n", pushed, data1);
    if (pushed != strlen(data1)) {
        printf("❌ 入栈测试失败：写入长度不匹配\n");
        return 1;
    }

    const char *data2 = "Second";
    pushed = stack_push(data2, strlen(data2));
    printf("入栈 %zu 字节: '%s'\n", pushed, data2);
    if (pushed != strlen(data2)) {
        printf("❌ 入栈测试失败：写入长度不匹配\n");
        return 1;
    }

    printf("当前栈深度: %zu\n", stack_depth());
    if (stack_depth() != strlen(data1) + strlen(data2)) {
        printf("❌ 入栈测试失败：栈深度不正确\n");
        return 1;
    }
    printf("✓ 入栈测试通过\n\n");

    // 测试查看栈顶
    printf("3. 测试查看栈顶\n");
    char peek_buf[16] = {0};
    size_t peeked = stack_peek(peek_buf, sizeof(peek_buf));
    printf("查看栈顶 %zu 字节: '%s'\n", peeked, peek_buf);
    if (peeked != strlen(data1) + strlen(data2) ||
        strncmp(peek_buf, "FirstSecond", peeked) != 0) {
        printf("❌ 查看栈顶测试失败：数据不匹配\n");
        return 1;
    }
    printf("✓ 查看栈顶测试通过\n\n");

    // 测试出栈操作（验证LIFO顺序）
    printf("4. 测试出栈操作（LIFO顺序）\n");
    char buf[16] = {0};
    size_t popped = stack_pop(buf, sizeof(buf));
    printf("出栈 %zu 字节: '%s'\n", popped, buf);
    if (popped != strlen(data1) + strlen(data2) ||
        strncmp(buf, "FirstSecond", popped) != 0) {
        printf("❌ 出栈测试失败：数据不匹配\n");
        return 1;
    }

    // 测试空栈操作
    printf("\n5. 测试空栈操作\n");
    memset(buf, 0, sizeof(buf));
    popped = stack_pop(buf, sizeof(buf));
    printf("从空栈出栈 %zu 字节\n", popped);
    if (popped != 0) {
        printf("❌ 空栈测试失败：应返回0字节\n");
        return 1;
    }

    // 测试栈已满情况
    printf("\n6. 测试栈已满情况\n");
    char full_data[20] = "0123456789ABCDEF01";  // 17字节，超过缓冲区大小
    pushed = stack_push(full_data, strlen(full_data));
    printf("尝试写入超过缓冲区大小的数据: %zu 字节\n", pushed);
    if (pushed >= sizeof(full_data)) {
        printf("❌ 栈已满测试失败：不应写入超过缓冲区大小的数据\n");
        return 1;
    }

    printf("\n最终栈深度: %zu\n", stack_depth());
    printf("✓ 所有栈测试通过！\n");
    return 0;
}
