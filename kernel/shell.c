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
#include "parse.h"
#include "../memory/pmm.h"
#include "../drivers/ports.h"

void shell_prompt() {
    kprint("KennyOS> ");
}

/* Helper: print "label: <number>\n" */
static void print_num(char *label, u32 n) {
    char buf[12];
    kprint(label);
    itoa((int)n, buf);
    kprint(buf);
    kprint("\n");
}



static void cmd_meminfo(void) {
    u32 used  = pmm_used_frames();
    u32 total = pmm_total_frames();

    print_num("Total frames: ", total);
    print_num("Used frames:  ", used);
    print_num("Free frames:  ", total - used);
    print_num("Used KiB:     ", used * 4);
    print_num("Free KiB:     ", (total - used) * 4);
}

static void cmd_memtest(void) {
    u32 a = alloc_frame();
    u32 b = alloc_frame();
    u32 c = alloc_frame();

    if (a == FRAME_ALLOC_FAIL || b == FRAME_ALLOC_FAIL || c == FRAME_ALLOC_FAIL) {
        kprint("alloc failed - out of memory\n");
        return;
    }

    print_num("alloc 1: ", a);
    print_num("alloc 2: ", b);
    print_num("alloc 3: ", c);

    free_frame(b);
    kprint("freed alloc 2\n");

    u32 d = alloc_frame();
    print_num("alloc 4: ", d);

    if (d == b) kprint("PASS: reused the freed frame\n");
    else        kprint("FAIL: did not reuse freed frame\n");

    /* Double free should be silently ignored, not corrupt the counter */
    free_frame(b);
    u32 before = pmm_used_frames();
    free_frame(b);
    if (pmm_used_frames() == before) kprint("PASS: double free rejected\n");
    else                             kprint("FAIL: double free changed counter\n");

    free_frame(a);
    free_frame(c);
    free_frame(d);
}


void shell_execute(char *input) {
    command_t cmd;
    parse_command(input, &cmd);

    if(cmd.argc == 0) {
        shell_prompt();
        return;
    }

    char *name = cmd.argv[0];

    if (strcmp(name, "help") == 0) {
        kprint("Available commands:\n");
        kprint("  help    - Show this message\n");
        kprint("  clear   - Clear the screen\n");
        kprint("  echo    - Print arguments\n");
        kprint("  meminfo - Show physical memory usage\n");
        kprint("  memtest - Exercise the frame allocator\n");
        kprint("  reboot  - Restart the system\n");

    } else if (strcmp(name, "clear") == 0) {
        clear_screen();

    } else if (strcmp(name, "echo") == 0) {

        for (int i = 1; i < cmd.argc; i++) {
            kprint(cmd.argv[i]);
            if (i < cmd.argc - 1) kprint(" ");
        }
        kprint("\n");

    } else if (strcmp(name, "reboot") == 0) {
        kprint("Rebooting...\n");
        port_byte_out(0x64, 0xFE);

    } else if (strcmp(name, "meminfo") == 0) {
        cmd_meminfo();

    } else if (strcmp(name, "memtest") == 0) {
        cmd_memtest();

    } else if (strlen(input) > 0) {
        kprint("Unknown command: ");
        kprint(input);
        kprint("\n");
    } 

    shell_prompt();
}
