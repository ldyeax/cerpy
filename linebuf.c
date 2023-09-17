/*
 * This file is part of linebuf.
 * This file is licensed under the terms of the MIT license, which should have
 * been provided to you with this file.
 */
#include "linebuf.h"
#include <ctype.h>
#include <assert.h>

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
    long bytesToCopy;

    if (newline_pos != NULL) {
        bytesToCopy = (newline_pos + 1L) - linebuf->buffer; /* +1 to include \n */
        assert(bytesToCopy > 0);

        *outline = (char *)calloc(1, bytesToCopy + 1);

        memcpy(*outline, linebuf->buffer, bytesToCopy);
        (*outline)[bytesToCopy] = '\0';

        linebuf->len -= bytesToCopy;

        memmove(linebuf->buffer, linebuf->buffer + bytesToCopy, linebuf->len);

        /* Remove trailing whitespace */
        while (bytesToCopy != 0 && isspace((*outline)[bytesToCopy - 1])) {
            (*outline)[bytesToCopy - 1] = '\0';
            bytesToCopy--;
        }

        return 1;
    }

    return 0;
}
