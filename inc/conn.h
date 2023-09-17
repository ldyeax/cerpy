#ifndef _CONN_H_INCLUDED
#define _CONN_H_INCLUDED
#include "buffer.h"

struct irc_conn {
    int fd;
    struct static_buffer buf;
};

struct irc_conn *irc_conn_open(const char *host, unsigned short port);
int irc_conn_do_io(struct irc_conn *conn);
ssize_t irc_conn_send(struct irc_conn *conn, const char *s);
ssize_t irc_conn_sendf(struct irc_conn *conn, const char *fmt, ...);
void irc_conn_close_free(struct irc_conn *conn);

#endif