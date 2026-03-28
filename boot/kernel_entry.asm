[bits 32]
[extern main]

[global _start]   ; ← ADD THIS LINE
_start:
    mov esp, 0x9000
    call main
    jmp $