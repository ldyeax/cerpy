#include <stdio.h>
#include "conn.h"
#include "parse.h"

#define IRC_NICK "cerpy"
#define IRC_USER "derp"
#define IRC_REAL "armchair"

struct irc_conn *g_Conn;

void process_line(struct irc_line *ln) {
    if (!strcmp(ln->command, "PING")) {
        irc_conn_sendf(g_Conn, "PONG :%s\r\n", ln->params[0]);
    }
}

int process_lines() {
    char *line;
    struct irc_line *ln;

    while (linebuf_pop(g_Conn->buf, &line)) {
        printf(">>> %s\n", line);
        ln = irc_parse(line);

        process_line(ln);

        free(line);
    }

    return 0;
}

int main(int argc, char *argv[]) {
    if ((g_Conn = irc_conn_open("irc.canternet.org", 6667)) == NULL) {
        return 1;
    }

    printf("%s\n", "Connected to IRC.");
    irc_conn_sendf(g_Conn, "NICK %s\r\n", IRC_NICK);
    irc_conn_sendf(g_Conn, "USER %s * * :%s\r\n", IRC_USER, IRC_REAL);

    while (1) {
        if (irc_conn_do_io(g_Conn) != 0) {
            break;
        }

        process_lines();
    }

    return 0;
}