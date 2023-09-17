#ifndef _STATIC_BUFFER_H
#define _STATIC_BUFFER_H

#define STATIC_BUFFER_CAPACITY 4096

struct static_buffer {
    char data[STATIC_BUFFER_CAPACITY];
    int count;
};

int static_buffer_append(struct static_buffer *buffer, char *data, int dataLength);
int static_buffer_append_str(struct static_buffer *buffer, char *str);
int static_buffer_pop_line(struct static_buffer *buffer, char *outBuf, int outLen);

#endif
