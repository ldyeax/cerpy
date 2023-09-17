#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include "conn.h"
#include "parse.h"

#define IRC_NICK "cerpy"
#define IRC_USER "derp"
#define IRC_REAL "armchair"

struct irc_conn *g_Conn;
int g_Running = 1;

#define UNUSED(P) ((void) P)


static void signal_handler(int signal) {
    UNUSED(signal);

    g_Running = 0;

    if (g_Conn != NULL && g_Conn->fd != -1) {
        close(g_Conn->fd);
        g_Conn->fd = -1;
    }

    fprintf(stderr, "Signal received; terminating\n");
}

void process_line(struct irc_line *ln) {
    if (!strcmp(ln->command, "PING")) {
        irc_conn_sendf(g_Conn, "PONG :%s\r\n", ln->params[0]);
    }
}

int process_lines() {
    struct irc_line *ln;
    char line[512];

    while (static_buffer_pop_line(&g_Conn->buf, line, 512)) {
        printf(">>> %s\n", line);
        ln = irc_parse(line);

        process_line(ln);
    }

    return 0;
}

int main(int argc, char *argv[]) {
    UNUSED(argc); UNUSED(argv);
    
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    if ((g_Conn = irc_conn_open("irc.canternet.org", 6667)) == NULL) {
        return 1;
    }

    printf("%s\n", "Connected to IRC.");
    irc_conn_sendf(g_Conn, "NICK %s\r\n", IRC_NICK);
    irc_conn_sendf(g_Conn, "USER %s * * :%s\r\n", IRC_USER, IRC_REAL);

    while (g_Running) {
        if (irc_conn_do_io(g_Conn) != 0) {
            break;
        }

        process_lines();
    }

    irc_conn_close_free(g_Conn);

    return 0;
}