[bits 16]
[org 0x7c00]

	jmp 0:start

start:
	; set register to 0
	xor ax, ax
	mov ds, ax ; this sets the data segment to 0, so it starts at the bottom of the memory (0x7c00)
	mov es, ax

	; point to video's memory address (VGA)
	mov ax, 0xb800
	mov es, ax

	; write H
	mov byte [es:0x0000], 'H'
	mov byte [es:0x0001], 0x4f ; white on black
	mov byte [es:0x0002], 'E'
	mov byte [es:0x0003], 0x4f
hang:
	cli
	hlt
	jmp hang

times 510-($-$$) db 0
dw 0xAA55

