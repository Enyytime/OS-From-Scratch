#include "keyboard.h"
#include "ports.h"
#include "screen.h"
#include "../cpu/isr.h"
#include "../kernel/util.h"
#include "../kernel/shell.h"

#define KB_BUFFER_SIZE 256

static char buffer[256];
static int buffer_index = 0;
static int shift_pressed = 0;
/**
 * @brief Scan Code Table (Set 1) for US QWERTY.
 * Index corresponds to the raw byte from port 0x60.
 */
const char sc_ascii[] = { 
    '?', '?', '1', '2', '3', '4', '5', '6',     // 00-07
    '7', '8', '9', '0', '-', '=', '?', '?',     // 08-0F
    'q', 'w', 'e', 'r', 't', 'y', 'u', 'i',     // 10-17
    'o', 'p', '[', ']', '?', '?', 'a', 's',     // 18-1F
    'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',     // 20-27
    '\'', '`', '?', '\\', 'z', 'x', 'c', 'v',    // 28-2F
    'b', 'n', 'm', ',', '.', '/', '?', '?',     // 30-37
    '?', ' '                                     // 38-39 (Space is 0x39)
};

const char sc_ascii_shift[] = {
    '?', '?', '!', '@', '#', '$', '%', '^',
    '&', '*', '(', ')', '_', '+', '?', '?',
    'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I',
    'O', 'P', '{', '}', '?', '?', 'A', 'S',
    'D', 'F', 'G', 'H', 'J', 'K', 'L', ':',
    '"', '~', '?', '|', 'Z', 'X', 'C', 'V',
    'B', 'N', 'M', '<', '>', '?', '?', '?',
    '?', ' '
};

static void keyboard_callback(registers_t regs) {
    unsigned char scancode = port_byte_in(0x60);

    if (scancode == 0x2A || scancode == 0x36) { 
        shift_pressed = 1; 
        return; 
    }
    
    if (scancode == 0xAA || scancode == 0xB6) {
        shift_pressed = 0;
        return;
    }
    /* Ignore 'Key Released' events (bit 7 is set) */
    if (scancode & 0x80) return;

    /* Handle Special Function Keys for Scrolling */
    if (scancode == 0x49) {         // Page Up
        scroll_up();
        return;
    } else if (scancode == 0x51) {   // Page Down
        scroll_down();
        return;
    }

    if (scancode == 0x0E) {
        if (buffer_index > 0) {
            buffer_index--;
            kprint_backspace();
        }
        return;
    }

    /* Handle Enter */
    if (scancode == 0x1C) {
        buffer[buffer_index] = '\0';
        buffer_index = 0;
        kprint("\n");
        shell_execute(buffer);
        return;
    }

    /* Map alphabetic/numeric keys */
    if (scancode <= 0x39) {
        char letter = shift_pressed ? sc_ascii_shift[(int)scancode]
                                    : sc_ascii[(int)scancode];

        /* Leave room for the null terminator — drop the key if full */
        if (buffer_index >= KB_BUFFER_SIZE - 1) return;

        buffer[buffer_index++] = letter;

        char str[2] = {letter, '\0'};
        kprint(str);
    }
}

void init_keyboard() {
   register_interrupt_handler(IRQ1, keyboard_callback); 
}