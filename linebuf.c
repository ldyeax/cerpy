/*
 * This file is part of linebuf.
 * This file is licensed under the terms of the MIT license, which should have
 * been provided to you with this file.
 */
#include "linebuf.h"

linebuf_t *linebuf_new() {
    linebuf_t *linebuf;

    linebuf = (linebuf_t *)calloc(1, sizeof(linebuf_t));

    linebuf->len = 0;
    linebuf->maxlen = 4096;
    linebuf->buffer = (char *)calloc(1, linebuf->maxlen);

    return linebuf;
}

void linebuf_destroy(linebuf_t *linebuf) {
    if (linebuf) {
        if (linebuf->buffer) {
            free(linebuf->buffer);
        }

        free(linebuf);
    }
}

void linebuf_append(linebuf_t *linebuf, char *data, int data_len) {
    int needed_size;

    needed_size = linebuf->len + data_len;

    if (needed_size > linebuf->maxlen) {
        linebuf->buffer = (char *)realloc(linebuf->buffer, needed_size);
        linebuf->maxlen = needed_size;
    }

    memcpy(linebuf->buffer + linebuf->len, data, data_len);
    linebuf->len = needed_size;
}

int linebuf_pop(linebuf_t *linebuf, char **outline) {
    char *newline_pos = (char *)memchr(linebuf->buffer, '\n', linebuf->len);
    int line_len;

    if (newline_pos != NULL) {
        line_len = (newline_pos - linebuf->buffer);
        *outline = (char *)calloc(1, line_len + 1);

        strncpy(*outline, linebuf->buffer, line_len);
        memmove(linebuf->buffer, newline_pos + 1, linebuf->len - line_len);
        linebuf->len -= line_len;
        return 1;
    }

    return 0;
}