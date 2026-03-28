#include "idt.h"
#include "../kernel/util.h"

idt_gate_t idt[IDT_ENTRIES];
idt_register_t idt_reg;
/**
 * @brief Configures a single gate (entry) in the IDT.
 * * This function takes a memory address (handler) and packs it into the 
 * 64-bit IDT gate structure required by the Intel 80386 processor.
 * * @param n       The interrupt number (0-255). This is the index in the IDT.
 * @param handler The 32-bit memory address of the assembly ISR stub.
 * * @details 
 * - **low_offset**: The lower 16 bits of the handler address.
 * - **sel**: The Kernel Code Segment selector (usually 0x08). This tells the CPU 
 * to switch to ring 0 (kernel mode).
 * - **always0**: A reserved byte that must be zero per Intel specifications.
 * - **flags**: Set to 0x8E. 
 * - 0x8 (1000b): Present bit (this gate is active).
 * - 0xE (1110b): 32-bit Interrupt Gate (disables interrupts on entry).
 * - **high_offset**: The higher 16 bits of the handler address.
 */
void set_idt_gate(int n, u32 handler) {
    /* Split the 32-bit address into two 16-bit halves */
    idt[n].low_offset = low_16(handler);
    
    /* Code segment selector - tells CPU we are in Kernel space */
    idt[n].sel = KERNEL_CS;
    
    /* Reserved byte must always be zero */
    idt[n].always0 = 0;
    
    /* Access flags: Present, Ring 0, 32-bit Interrupt Gate */
    idt[n].flags = 0x8E; 
    
    /* The other half of the 32-bit address */
    idt[n].high_offset = high_16(handler);
}

/**
 * @brief Loads the IDT into the CPU's internal IDTR register.
 * * This function prepares the IDT pointer (base and limit) and executes
 * the 'lidt' assembly instruction to officially enable the interrupt table.
 * * @details
 * - **idt_reg.base**: The linear address of the 'idt' array in RAM.
 * - **idt_reg.limit**: The size of the table in bytes minus one. This acts 
 * as a safety boundary for the CPU.
 * * @note We use inline assembly (__asm__) because C cannot directly access 
 * internal CPU registers like the IDTR.
 */
void set_idt() {
    /* Pointer to the start of the IDT array */
    idt_reg.base = (u32) &idt;
    
    /* Size of the entire table (256 entries * size of each entry) - 1 */
    idt_reg.limit = IDT_ENTRIES * sizeof(idt_gate_t) - 1;
    
    /* Load the IDT Register (lidt) using the address of our pointer struct */
    __asm__ __volatile__("lidtl (%0)" : : "r" (&idt_reg));
}