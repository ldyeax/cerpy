/*
 * This file is part of linebuf.
 * This file is licensed under the terms of the MIT license, which should have
 * been provided to you with this file.
 */
#include <ctype.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "linebuf.h"

int static_buffer_append(struct static_buffer *buffer, char *data, int dataLength) {
    assert(buffer != NULL);
    assert(data != NULL);

    assert(dataLength > 0);

    int newCount = buffer->count + dataLength;

    /* Assertion for development, if statement for production. Better to have it fail in prod than crash in prod. */
    assert(newCount < STATIC_BUFFER_CAPACITY);

    if (newCount > STATIC_BUFFER_CAPACITY) {
        return 0;
    }

    memcpy(buffer->data + buffer->count, data, dataLength);

    buffer->count = newCount;

    return 1;
}

int static_buffer_append_str(struct static_buffer *buffer, char *str) {
    return static_buffer_append(buffer, str, strlen(str));
}

int static_buffer_pop_line(struct static_buffer *buffer, char *outBuf, int outLen) {
    int bytesToCopy; /* Number of bytes to copy from the buffer to the output */
    char *newlinePtr = (char *)memchr(buffer->data, '\n', buffer->count);

    assert(buffer != NULL);
    assert(outBuf != NULL);
    assert(outLen > 0);

    if (newlinePtr == NULL) {
        return 0;
    }

    bytesToCopy = (newlinePtr + 1) - buffer->data; /* newlinePtr + 1 means we will include the newline in the copy. */

    assert(bytesToCopy > 0);

    /* Same situation with the assertion as with the other function above. */
    assert((bytesToCopy + 1) <= outLen);

    if ((bytesToCopy + 1) > outLen) { /* +1 for the null terminator we add. */
        return 0;
    }

    memcpy(outBuf, buffer->data, bytesToCopy);
    outBuf[bytesToCopy] = '\0';

    buffer->count -= bytesToCopy;

    assert(buffer->count >= 0);

    /* memmove() of 0 doesn't fail, but there's no reason to try it if we don't have to. */
    if (buffer->count > 0) {
        memmove(buffer->data, buffer->data + bytesToCopy, buffer->count);
    }

    /* Remove trailing whitespace */
    while (bytesToCopy != 0 && isspace(outBuf[bytesToCopy - 1])) {
        outBuf[bytesToCopy - 1] = '\0';
        bytesToCopy--;
    }

    return 1;
}
