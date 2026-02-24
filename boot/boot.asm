[bits 16]
[org 0x7c00]

start:
    mov ah, 0x0e      ; BIOS teletype function
    mov al, '!'       ; Character to print
    int 0x10          ; Print character
    cli               ; Disable interrupts
    hlt               ; Halt CPU

times 510-($-$$) db 0
dw 0xAA55             ; Boot signature
