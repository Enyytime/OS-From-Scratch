#ifndef PARSE_H
#define PARSE_H

#define MAX_ARGS    8
#define MAX_ARG_LEN 64

typedef struct {
    int argc;
    char argv[MAX_ARGS][MAX_ARG_LEN];
} command_t;

void parse_command(const char *input, command_t *cmd);

#endif