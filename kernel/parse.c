#include "parse.h"
#include "util.h"

void parse_command(const char *input, command_t *cmd) {
    cmd->argc = 0;
    int i = 0;

    while (input[i] != '\0' && cmd->argc < MAX_ARGS) {

        // skip leading whitespaces
        while (input[i] == ' ' || input[i] == '\t') {
            i++;
        }

        if (input[i] == '\0') {
            break;
        }

        // copy one character each
        int j = 0;
        while(input[i] != '\0' && input[i] != ' ' && input[i] != '\t') {

            if (j < MAX_ARG_LEN - 1) {
                cmd->argv[cmd->argc][j++] = input[i];
            }
            i++;
        }

        cmd->argv[cmd->argc][j] = '\0';
        cmd->argc++;
    }
}