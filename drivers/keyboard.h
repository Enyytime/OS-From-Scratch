#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "../cpu/types.h"

/**
 * @brief The IRQ number for the keyboard on the PIC.
 * IRQ 1 is mapped to IDT index 33 (0x21).
 */
#define IRQ1 33

/**
 * @brief Initializes the keyboard driver.
 * * Registers the keyboard_callback function with the Interrupt 
 * Descriptor Table (IDT) so the CPU knows where to jump 
 * when a key is pressed.
 */
void init_keyboard();

#endif