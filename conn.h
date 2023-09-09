#ifndef _CONN_H_INCLUDED
#define _CONN_H_INCLUDED
#include "linebuf.h"

struct irc_conn {
    int fd;
    linebuf_t *buf;
};

struct irc_conn *irc_conn_open(const char *host, unsigned short port);
int irc_conn_do_io(struct irc_conn *conn);
int irc_conn_send(struct irc_conn *conn, const char *s);
int irc_conn_sendf(struct irc_conn *conn, const char *fmt, ...);
void irc_conn_close_free(struct irc_conn *conn);

#endif