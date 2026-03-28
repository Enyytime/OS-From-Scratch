/**
 * @file ports.c
 * @brief Low-level I/O port communication functions.
 * * These functions provide an interface between C and the x86 'in' and 'out' 
 * assembly instructions, allowing the kernel to communicate with hardware 
 * devices via their specific I/O ports.
 */

#include "ports.h"

/**
 * @brief Reads a single byte from the specified hardware port.
 * * @param port The 16-bit I/O port address to read from (mapped to DX).
 * @return unsigned char The 8-bit data byte received from the port (from AL).
 * * @details Uses inline assembly to execute the 'in' instruction. 
 * The port address is loaded into DX, and the result is captured from AL.
 */
unsigned char port_byte_in (unsigned short port) {
    unsigned char result;
    /* "=a" (result) means: put the value of AL into the 'result' variable 
     * "d" (port) means: load the 'port' variable into the DX register */
    __asm__("in %%dx, %%al" : "=a" (result) : "d" (port));
    return result;
}

/**
 * @brief Writes a single byte to the specified hardware port.
 * * @param port The 16-bit I/O port address to write to (mapped to DX).
 * @param data The 8-bit data byte to send to the hardware (mapped to AL).
 */
void port_byte_out (unsigned short port, unsigned char data) {
    /* "a" (data) loads AL with our byte, "d" (port) loads DX with the port.
     * No '=' is used because we aren't returning a value to C. */
    __asm__("out %%al, %%dx" : : "a" (data), "d" (port));
}

/**
 * @brief Reads a 16-bit word from the specified hardware port.
 * * @param port The 16-bit I/O port address to read from.
 * @return unsigned short The 16-bit data word received (from AX).
 */
unsigned short port_word_in (unsigned short port) {
    unsigned short result;
    __asm__("in %%dx, %%ax" : "=a" (result) : "d" (port));
    return result;
}

/**
 * @brief Writes a 16-bit word to the specified hardware port.
 * * @param port The 16-bit I/O port address to write to.
 * @param data The 16-bit data word to send (mapped to AX).
 */
void port_word_out (unsigned short port, unsigned short data) {
    __asm__("out %%ax, %%dx" : : "a" (data), "d" (port));
}