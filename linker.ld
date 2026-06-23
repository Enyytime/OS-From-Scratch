ENTRY(_start)

SECTIONS {
    . = 0x1000;

    .text : { *(.text) }
    .rodata : { *(.rodata*) }
    .data : { *(.data) }
    .bss : { *(.bss) *(COMMON) }

    . = ALIGN(4096);
    kernel_end = .;
}