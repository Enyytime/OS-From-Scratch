/**
 * @file shell.c
 * @brief Simple command-line shell for the kernel.
 *
 * Provides a basic interactive shell that reads user input from
 * the keyboard driver and executes built-in commands.
 */

#include "shell.h"
#include "util.h"
#include "../drivers/screen.h"
#include "../drivers/ports.h"

void shell_prompt() {
    kprint("KennyOS> ");
}

void shell_execute(char *input) {
    if (strcmp(input, "help") == 0) {
        kprint("Available commands:\n");
        kprint("  help   - Show this message\n");
        kprint("  clear  - Clear the screen\n");
        kprint("  reboot - Restart the system\n");

    } else if (strcmp(input, "clear") == 0) {
        clear_screen();

    } else if (strcmp(input, "reboot") == 0) {
        kprint("Rebooting...\n");
        port_byte_out(0x64, 0xFE);

    } else if (strlen(input) > 0) {
        kprint("Unknown command: ");
        kprint(input);
        kprint("\n");
    }

    shell_prompt();
}
