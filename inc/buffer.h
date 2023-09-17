#ifndef _STATIC_BUFFER_H
#define _STATIC_BUFFER_H
#include <unistd.h>

#define STATIC_BUFFER_CAPACITY 4096

struct static_buffer {
    char data[STATIC_BUFFER_CAPACITY];
    size_t count;
};

int static_buffer_append(struct static_buffer *buffer, char *data, size_t dataLength);
int static_buffer_append_str(struct static_buffer *buffer, char *str);
int static_buffer_pop_line(struct static_buffer *buffer, char *outBuf, int outLen);

#endif
