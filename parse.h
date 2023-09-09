#define IRC_MAX_NICK 18
#define IRC_MAX_USER 18
#define IRC_MAX_HOST 32

#define IRC_MAX_COMMAND  16
#define IRC_MAX_PARAM    512
#define IRC_MAX_PARAMS   16

#define SPLIT_MAX_PARTS 8
#define SPLIT_MAX_PART  32

struct irc_hostmask {
    char nick[IRC_MAX_NICK + 1];
    char user[IRC_MAX_USER + 1];
    char host[IRC_MAX_HOST + 1];
};

struct irc_line {
    struct irc_hostmask hostmask;
    char command[IRC_MAX_COMMAND + 1];
    int numParams;
    char params[IRC_MAX_PARAMS][IRC_MAX_PARAM];
};

struct split_struct {
    int numParts;
    char *parts[SPLIT_MAX_PARTS];
};

struct irc_line *irc_parse(const char *line);
int dynamic_split(const char *str, struct split_struct *split);
/* bool static_split(const char *str, struct split_struct *split); */
