#include "keyboard.h"
#include "ports.h"
#include "screen.h"
#include "../cpu/isr.h"
#include "../kernel/util.h"

/**
 * @brief Scan Code Table (Set 1) for US QWERTY.
 * Index corresponds to the raw byte from port 0x60.
 */
const char sc_ascii[] = { 
    '?', '?', '1', '2', '3', '4', '5', '6',     // 00-07
    '7', '8', '9', '0', '-', '=', '?', '?',     // 08-0F
    'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I',     // 10-17
    'O', 'P', '[', ']', '?', '?', 'A', 'S',     // 18-1F
    'D', 'F', 'G', 'H', 'J', 'K', 'L', ';',     // 20-27
    '\'', '`', '?', '\\', 'Z', 'X', 'C', 'V',    // 28-2F
    'B', 'N', 'M', ',', '.', '/', '?', '?',     // 30-37
    '?', ' '                                     // 38-39 (Space is 0x39)
};

static void keyboard_callback(registers_t regs) {
    unsigned char scancode = port_byte_in(0x60);

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
        kprint_backspace(); 
        return;
    }

    /* Handle Enter */
    if (scancode == 0x1C) {
        kprint("\n");
        return;
    }

    /* Map alphabetic/numeric keys */
    if (scancode <= 0x39) {
        char letter = sc_ascii[(int)scancode];
        char str[2] = {letter, '\0'}; // kprint expects a string
        kprint(str);
    }
}

void init_keyboard() {
   register_interrupt_handler(IRQ1, keyboard_callback); 
}