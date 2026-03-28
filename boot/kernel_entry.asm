[bits 32]
[extern main] ; Define calling point. Must have same name as kernel.c 'main' function

_start:
    mov esp, 0x9000    ; <--- ADD THIS LINE! Set the stack safely away from your code
    call main
    jmp $

