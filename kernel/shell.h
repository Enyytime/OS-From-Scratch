#ifndef SHELL_H
#define SHELL_H

/**
 * @brief Parses and executes a command string.
 * @param input The null-terminated command string from the keyboard buffer.
 */
void shell_execute(char *input);

/**
 * @brief Prints the shell prompt to the screen.
 */
void shell_prompt();

#endif
