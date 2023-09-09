/*
 * This file is part of linebuf.
 * This file is licensed under the terms of the MIT license, which should have
 * been provided to you with this file.
 */
/** \file linebuf.h
 * This file  contains all of the functions used to create, manipulate,
 * and destroy line buffers.
 */
#ifndef _LINEBUF_H_INCLUDED
#define _LINEBUF_H_INCLUDED
#include <string.h>
#include <stdlib.h>

/**
 * \struct linebuf
 * \brief Represents a line buffer.
 *
 * \var linebuf::buffer
 * This member contains the actual data in the buffer.
 * It is not null-terminated.
 *
 * \var linebuf::len
 * This member contains the current number of bytes
 * in the buffer.
 *
 * \var linebuf::maxlen
 * This member contains the maximum number of bytes
 * the buffer can hold before we need to grow the
 * memory allocation.
 */
typedef struct linebuf {
    char *buffer;
    int len;
    int maxlen;
} linebuf_t;

/**
 * Create a new line buffer.
 *
 * \return New linebuf object
 */
linebuf_t *linebuf_new();

/**
 * Destroy a line buffer (free it and its contents from memory).
 * Passing a null pointer will have no effect.
 *
 * \param linebuf The linebuf object to destroy.
 */
void linebuf_destroy(linebuf_t *linebuf);

/**
 * Append arbitrary data to a line buffer.
 * This data may contain an incomplete line, a complete line, multiple
 * complete lines, or multiple complete lines followed by an incomplete
 * line.
 *
 * \param linebuf The linebuf object to which we wish to append data.
 * \param data A pointer to the start of the data we wish to append.
 * \param data_len The number of bytes we wish to append to the buffer.
 */
void linebuf_append(linebuf_t *linebuf, char *data, int data_len);

/**
 * Pop a line from a line buffer, if it contains one or more.
 *
 * \param linebuf The linebuf we wish to pop a line from.
 * \param outline A pointer to a char pointer, which will be allocated
 *                and filled with a null-terminated string containing the
 *                popped line.
 * \return 1 if there was a line that we popped, 0 if there was no
 *         line or only part of a line, -1 on error.
 */
int linebuf_pop(linebuf_t *linebuf, char **outline);

#endif