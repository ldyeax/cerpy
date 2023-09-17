#include "conn.h"
#include <errno.h>
#include <netdb.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>

/* Investigate this - possibly unavailable in c89 */
void herror(const char *msg);
int vsnprintf (char * s, size_t n, const char * format, va_list arg );

struct irc_conn *irc_conn_open(const char *host, unsigned short port) {
    int fd;
    struct irc_conn *conn;
    struct hostent *res;
    struct sockaddr_in saddr;

    conn = (struct irc_conn *) malloc(sizeof(struct irc_conn));

    if (!conn) {
        fprintf(stderr, "irc_conn_open(): failed to allocate memory\n");
        goto error;
    }

    res = gethostbyname(host);

    if (res == NULL) {
        herror("irc_conn_open(): gethostbyname() says:"); /* Print gethostbyname() error */
        goto error;
    }

    if ((fd = socket(PF_INET, SOCK_STREAM, 0)) == -1) {
        fprintf(stderr, "irc_conn_open(): socket() says: %s\n", strerror(errno));
        goto error;
    }

    memset(&saddr, 0, sizeof(saddr));

    saddr.sin_family = AF_INET;
    memcpy(&saddr.sin_addr, (struct in_addr *) res->h_addr_list[0], sizeof(struct in_addr));
    saddr.sin_port = htons(port);

    if (connect(fd, (struct sockaddr *) &saddr, sizeof(struct sockaddr_in)) == -1) {
        fprintf(stderr, "irc_conn_open(): connect() says: %s\n", strerror(errno));
        goto error;
    }

    conn->fd = fd;

    return conn;

error:
    if (conn) {
        irc_conn_close_free(conn);
    }

    return NULL;
}

ssize_t irc_conn_send(struct irc_conn *conn, const char *s) {
    ssize_t sent;
    size_t to_send;
    size_t total_sent;

    if (!conn || conn->fd == -1) {
        fprintf(stderr, "irc_conn_send(): got an invalid IRC connection\n");
        return -1;
    }

    to_send = strlen(s);
    total_sent = 0;

    while (total_sent < to_send) {
        sent = send(conn->fd, s, (to_send - total_sent), 0);

        if (sent == -1) {
            fprintf(stderr, "irc_conn_send(): send() says: %s\n", strerror(errno));
            return -1;
        }

        if (sent == 0) {
            fprintf(stderr, "irc_conn_send(): send() says: connection closed\n");
            return -1;
        }

        total_sent += sent;
    }

    printf("<<< %s", s);

    return (ssize_t) total_sent;
}

ssize_t irc_conn_sendf(struct irc_conn *conn, const char *fmt, ...) {
    char buf[512];
    va_list args;

    va_start(args, fmt);
    vsnprintf(buf, 512, fmt, args);
    va_end(args);

    return irc_conn_send(conn, buf);
}

int irc_conn_do_io(struct irc_conn *conn) {
    char buf[STATIC_BUFFER_CAPACITY];
    ssize_t rc;

    /* receive as much as we can into the buffer */
    rc = recv(conn->fd, buf, STATIC_BUFFER_CAPACITY - conn->buf.count, 0);

    if (rc == -1) {
        fprintf(stderr, "irc_conn_do_io(): recv() says: %s\n", strerror(errno));
        return -1;
    }

    if (rc == 0) {
        fprintf(stderr, "irc_conn_do_io(): recv() says: %s\n", strerror(errno));
        return -1;
    }

    static_buffer_append(&conn->buf, buf, rc);

    return 0;
}

void irc_conn_close_free(struct irc_conn *conn) {
    if (conn) {
        if (conn->fd != -1) {
            close(conn->fd);
        }

        free(conn);
    }
}
