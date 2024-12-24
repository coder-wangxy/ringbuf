#include <stdio.h>
#include <string.h>
#include "ringbuf.h"

// Define a ring buffer of size 16 (must be power of 2)
RINGBUF_DEFINE(mybuf, 16);

int main(void) {
    char input_data[] = "hello";
    char output_data[16];
    size_t written, read;

    // Write data to buffer
    written = ringbuf_put(&mybuf, input_data, strlen(input_data));
    printf("Put %zu bytes to ring buffer.\n", written);

    // Read data from buffer
    memset(output_data, 0, sizeof(output_data));
    read = ringbuf_get(&mybuf, output_data, sizeof(output_data));
    printf("Got %zu bytes from ring buffer: '%s'\n", read, output_data);

    return 0;
}
