[bits 32]
[extern main]

[global _start]   ; ← ADD THIS LINE
_start:
    mov esp, 0x90000
    call main
    jmp $