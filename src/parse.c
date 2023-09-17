#include "parse.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define SKIP_SPACES(P) { while (*P && isspace(*P)) { P++; } }
#define COUNT_AND_SKIP_NON_SPACES(P, C) { while (*P && !isspace(*P)) { P++; C++; } }

void irc_parse_hostmask(struct irc_hostmask *hostmask, const char *raw, size_t rawLength) {
    const char *atSymbolPtr;
    const char *nickSepPtr;
    size_t nickLength;
    size_t userLength;
    size_t hostLength;

    assert(raw != NULL);
    assert(rawLength > 0);

    atSymbolPtr = memchr(raw, '@', rawLength);

    if (atSymbolPtr == NULL) { /* No @ */
        assert(rawLength <= IRC_MAX_HOST);

        strncpy(hostmask->host, raw, rawLength);
        return;
    }

    nickSepPtr = memchr(raw, '!', rawLength);

    assert(nickSepPtr < atSymbolPtr);

    hostLength = rawLength - (atSymbolPtr + 1 - raw);

    assert(hostLength <= IRC_MAX_HOST);

    if (nickSepPtr != NULL) {
        nickLength = (nickSepPtr - raw);
        userLength = (atSymbolPtr - 1 - nickSepPtr);

        assert(nickLength <= IRC_MAX_NICK);
        assert(userLength <= IRC_MAX_USER);

        strncpy(hostmask->nick, raw, (nickLength > IRC_MAX_NICK ? IRC_MAX_NICK : nickLength));
        strncpy(hostmask->user, nickSepPtr + 1, (userLength > IRC_MAX_USER ? IRC_MAX_USER : userLength));
        strncpy(hostmask->host, atSymbolPtr + 1, (hostLength > IRC_MAX_HOST ? IRC_MAX_HOST : hostLength));
    } else {
        nickLength = (atSymbolPtr - raw);
        assert(nickLength <= IRC_MAX_NICK);

        strncpy(hostmask->nick, raw, (nickLength > IRC_MAX_NICK ? IRC_MAX_NICK : nickLength));
        strncpy(hostmask->host, atSymbolPtr + 1, (hostLength > IRC_MAX_HOST ? IRC_MAX_HOST : hostLength));
    }
}

/* this is not even remotely reentrant, someone change it to take a ptr to parsed instead */
struct irc_line *irc_parse(const char *line) {
    static struct irc_line parsed;

    const char *ptr;
    const char *hostmaskPtr;
    size_t hostmaskLength;
    const char *numericPtr;
    size_t numericLength;
    int paramCount;
    int paramLength;    
    const char *paramPtr;

    hostmaskLength = 0;
    numericLength = 0;
    paramCount = 0;

    memset(&parsed, 0, sizeof(parsed));

    /* Do we have a hostmask at the start? */
    if (*line == ':') {
        hostmaskPtr = line + 1;
        ptr = line + 1;

        /* Advance ptr past the hostmask portion. */
        COUNT_AND_SKIP_NON_SPACES(ptr, hostmaskLength)

        /* Skip the whitespace after the hostmask */
        SKIP_SPACES(ptr)

        irc_parse_hostmask(&(parsed.hostmask), hostmaskPtr, hostmaskLength);

        numericPtr = ptr;
    } else { /* No hostmask at the start */
        numericPtr = line;
    }

    ptr = numericPtr;

    /* Advance ptr past the command / numeric. */
    COUNT_AND_SKIP_NON_SPACES(ptr, numericLength)

    /* Skip the whitespace after the numeric */
    SKIP_SPACES(ptr)

    /* This assertion will be hit in development if we ever encounter something unseemly */
    assert(numericLength < IRC_MAX_COMMAND);

    /* copy the numeric/param in here, making sure not to overflow the buffer (no assertion in production). */
    strncpy(parsed.command, numericPtr, (numericLength > IRC_MAX_COMMAND ? IRC_MAX_COMMAND : numericLength));

    while (*ptr) {
        paramLength = 0;
        paramPtr = ptr;

        if (ptr[0] == ':') {
            /* Stop at the null-terminator at the end of the line here, or when we hit IRC_MAX_PARAM */
            strncpy(parsed.params[paramCount], ptr + 1, IRC_MAX_PARAM);
            paramCount++;
            break;
        }

        while (*ptr && !isspace(*ptr)) {
            paramLength++;
            ptr++;
        }

        COUNT_AND_SKIP_NON_SPACES(ptr, paramLength)

        SKIP_SPACES(ptr)

        /* This assertion will be hit in development if we ever encounter something unseemly */
        assert(paramLength < IRC_MAX_PARAM);

        /* No assertion in production, so be sure we don't overflow the parameter buffer */
        strncpy(parsed.params[paramCount], paramPtr, (paramLength > IRC_MAX_PARAM ? IRC_MAX_PARAM : paramLength));
        paramCount++;

        /* Development assertion to ensure we don't have too many params. */
        assert(paramCount < IRC_MAX_PARAMS);

        /* Or in production, just bail out. */
        if (paramCount == IRC_MAX_PARAMS) {
            break;
        }
    }

    parsed.numParams = paramCount;
    
    return &parsed;
}

/* untested, someone needs to write a free function too to go over the parts, see if they're null, and free 'em */
int dynamic_split(const char *str, struct split_struct *split) {
    int partIndex;
    int partLength;
    const char *partPtr;
    char *outPtr;

    assert(str != NULL);
    assert(split != NULL);

    memset(split->parts, 0, sizeof(char *) * SPLIT_MAX_PARTS);

    partIndex = 0;

    while (*str) {
        SKIP_SPACES(str)

        partLength = 0;
        partPtr = str;

        outPtr = NULL;

        COUNT_AND_SKIP_NON_SPACES(str, partLength)
        outPtr = (char *) malloc(partLength + 1);

        if (!outPtr) {
            fprintf(stderr, "dynamic_split(): failed to allocate outPtr\n");
            return 0;
        }

        strncpy(outPtr, partPtr, partLength);

        partIndex++;

        if (partIndex == SPLIT_MAX_PARTS) {
            return 0;
        }
    }

    split->numParts = partIndex;

    return 1;
}


/*
bool static_split(const char *str, struct split_struct *split) {
    int partIndex;
    int partLength;
    const char *partPtr;

    assert(str != NULL);
    assert(split != NULL);

    partIndex = 0;

    while (*str) {
        SKIP_SPACES(str);

        partLength = 0;
        partPtr = str;

        COUNT_AND_SKIP_NON_SPACES(str, partLength);

        if (partLength > SPLIT_MAX_PART) {
            return false;
        }

        strncpy(split->parts[partIndex], partPtr, partLength);

        partIndex++;

        if (partIndex == SPLIT_MAX_PARTS) {
            return false;
        }
    }

    split->numParts = partIndex;

    return true;
}
*/
