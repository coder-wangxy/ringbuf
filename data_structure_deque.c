#include <stdio.h>
#include <string.h>
#include "ringbuf.h"

// 定义16字节的双端队列缓冲区
RINGBUF_DEFINE(deque_buf, 16);

// 从头部插入
static size_t deque_push_front(const void *data, size_t length) {
    size_t current_length = ringbuf_length(&deque_buf);
    
    // 检查是否有足够空间
    if (current_length + length > 16) {
        return 0;
    }
    
    // 如果当前有数据，需要先取出
    char temp[16] = {0};
    if (current_length > 0) {
        ringbuf_get(&deque_buf, temp, current_length);
    }
    
    // 先插入新数据
    size_t written = ringbuf_put(&deque_buf, data, length);
    
    // 再插入原有数据
    if (current_length > 0) {
        ringbuf_put(&deque_buf, temp, current_length);
    }
    
    return written;
}

// 从尾部插入
static size_t deque_push_back(const void *data, size_t length) {
    return ringbuf_put(&deque_buf, data, length);
}

// 从头部删除
static size_t deque_pop_front(void *buffer, size_t size) {
    return ringbuf_get(&deque_buf, buffer, size);
}

// 从尾部删除
static size_t deque_pop_back(void *buffer, size_t size) {
    size_t length = ringbuf_length(&deque_buf);
    if (length == 0) return 0;
    
    // 计算要删除的大小
    size_t to_pop = (size < length) ? size : length;
    size_t to_keep = length - to_pop;
    
    // 取出所有数据
    char temp[16] = {0};
    ringbuf_get(&deque_buf, temp, length);
    
    
    // 复制要返回的数据（尾部数据）
    memcpy(buffer, temp + to_keep, to_pop);
    
    // 保留剩余数据
    if (to_keep > 0) {
        ringbuf_put(&deque_buf, temp, to_keep);
    }
    
    return to_pop;
}

// 从头部查看
static size_t deque_peek_front(void *buffer, size_t size) {
    return ringbuf_peek(&deque_buf, buffer, size);
}

// 从尾部查看
static size_t deque_peek_back(void *buffer, size_t size) {
    size_t length = ringbuf_length(&deque_buf);
    if (length == 0) return 0;
    
    // 取出所有数据以查看尾部
    char temp[16] = {0};
    size_t peeked = ringbuf_peek(&deque_buf, temp, length);
    
    // 计算要查看的大小
    size_t to_peek = (size < peeked) ? size : peeked;
    
    // 复制尾部数据
    memcpy(buffer, temp + peeked - to_peek, to_peek);
    
    return to_peek;
}

// 获取双端队列长度
static size_t deque_length(void) {
    return ringbuf_length(&deque_buf);
}

int main(void) {
    printf("=== 双端队列(Deque)测试 ===\n\n");
    
    // 测试初始化状态
    printf("1. 测试初始化状态\n");
    printf("初始队列长度: %zu\n", deque_length());
    if (deque_length() != 0) {
        printf("❌ 初始化测试失败：队列长度不为0\n");
        return 1;
    }
    printf("✓ 初始化测试通过\n\n");
    
    // 测试从尾部插入
    printf("2. 测试从尾部插入\n");
    const char *data1 = "First";
    size_t pushed = deque_push_back(data1, strlen(data1));
    printf("尾部插入 %zu 字节: '%s'\n", pushed, data1);
    if (pushed != strlen(data1)) {
        printf("❌ 尾部插入测试失败：写入长度不匹配\n");
        return 1;
    }
    
    // 测试从头部插入
    printf("\n3. 测试从头部插入\n");
    const char *data2 = "Second";
    pushed = deque_push_front(data2, strlen(data2));
    printf("头部插入 %zu 字节: '%s'\n", pushed, data2);
    if (pushed != strlen(data2)) {
        printf("❌ 头部插入测试失败：写入长度不匹配\n");
        return 1;
    }
    
    printf("当前队列长度: %zu\n", deque_length());
    if (deque_length() != strlen(data1) + strlen(data2)) {
        printf("❌ 插入测试失败：队列长度不正确\n");
        return 1;
    }
    printf("✓ 插入测试通过\n\n");
    
    // 测试从头部和尾部查看
    printf("4. 测试从两端查看\n");
    char peek_buf[16] = {0};
    size_t peeked = deque_peek_front(peek_buf, sizeof(peek_buf));
    printf("从头部查看 %zu 字节: '%s'\n", peeked, peek_buf);
    if (peeked != strlen(data1) + strlen(data2) || 
        strncmp(peek_buf, "SecondFirst", peeked) != 0) {
        printf("❌ 头部查看测试失败：数据不匹配\n");
        return 1;
    }
    
    memset(peek_buf, 0, sizeof(peek_buf));
    peeked = deque_peek_back(peek_buf, sizeof(peek_buf));
    printf("从尾部查看 %zu 字节: '%s'\n", peeked, peek_buf);
    if (peeked != strlen(data1) + strlen(data2) || 
        strncmp(peek_buf, "SecondFirst", peeked) != 0) {
        printf("❌ 尾部查看测试失败：数据不匹配\n");
        return 1;
    }
    printf("✓ 查看测试通过\n\n");
    
    // 测试从头部删除
    printf("5. 测试从两端删除\n");
    char buf[16] = {0};
    size_t popped = deque_pop_front(buf, sizeof(buf));
    printf("从头部删除 %zu 字节: '%s'\n", popped, buf);
    if (popped != strlen(data1) + strlen(data2) || 
        strncmp(buf, "SecondFirst", popped) != 0) {
        printf("❌ 头部删除测试失败：数据不匹配\n");
        return 1;
    }
    
    // 再次插入数据用于尾部删除测试
    deque_push_back(data1, strlen(data1));
    
    // 测试从尾部删除
    memset(buf, 0, sizeof(buf));
    popped = deque_pop_back(buf, sizeof(buf));
    printf("从尾部删除 %zu 字节: '%s'\n", popped, buf);
    if (popped != strlen(data1) || 
        strncmp(buf, "First", popped) != 0) {
        printf("❌ 尾部删除测试失败：数据不匹配\n");
        return 1;
    }
    printf("✓ 删除测试通过\n\n");
    
    // 测试空队列操作
    printf("6. 测试空队列操作\n");
    memset(buf, 0, sizeof(buf));
    popped = deque_pop_front(buf, sizeof(buf));
    printf("从空队列头部删除 %zu 字节\n", popped);
    if (popped != 0) {
        printf("❌ 空队列头部删除测试失败：应返回0字节\n");
        return 1;
    }
    
    popped = deque_pop_back(buf, sizeof(buf));
    printf("从空队列尾部删除 %zu 字节\n", popped);
    if (popped != 0) {
        printf("❌ 空队列尾部删除测试失败：应返回0字节\n");
        return 1;
    }
    printf("✓ 空队列测试通过\n\n");
    
    // 测试队列已满情况
    printf("7. 测试队列已满情况\n");
    char full_data[20] = "0123456789ABCDEF01"; // 17字节，超过缓冲区大小
    pushed = deque_push_back(full_data, strlen(full_data));
    printf("尝试从尾部写入超过缓冲区大小的数据: %zu 字节\n", pushed);
    if (pushed >= sizeof(full_data)) {
        printf("❌ 队列已满测试失败：不应写入超过缓冲区大小的数据\n");
        return 1;
    }
    
    pushed = deque_push_front(full_data, strlen(full_data));
    printf("尝试从头部写入超过缓冲区大小的数据: %zu 字节\n", pushed);
    if (pushed >= sizeof(full_data)) {
        printf("❌ 队列已满测试失败：不应写入超过缓冲区大小的数据\n");
        return 1;
    }
    printf("✓ 队列已满测试通过\n\n");
    
    printf("最终队列长度: %zu\n", deque_length());
    printf("✓ 所有双端队列测试通过！\n");
    return 0;
}
