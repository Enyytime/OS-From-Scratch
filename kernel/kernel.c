#include "../cpu/idt.h"
#include "../cpu/isr.h"
#include "../drivers/screen.h"
#include "../drivers/keyboard.h"

void main() {
    // 1. Install the ISRs (fills the 'idt' array with handler addresses)
    isr_install(); 

    // 2. Load the IDT into the CPU (Your function in idt.c)
    set_idt(); 

    // 3. Setup keyboard
    init_keyboard();

    // 4. Enable interrupts
    asm volatile("sti");

    kprint("Kenny's OS is active, bruh.\n");
}