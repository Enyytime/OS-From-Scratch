[extern isr_handler]
[extern irq_handler]

; Common ISR Code

; @brief Common entry point for all ISRs.
;
; This stub performs the following "Sandwich" workflow:
; 1. Save: Pushes all general-purpose registers and the User Data Segment.
; 2. Switch: Changes the CPU's "signposts" (DS, ES, FS, GS) to the Kernel Data Segment (0x10).
; 3. Execute: Calls the C function `isr_handler` to process the interrupt logic.
; 4. Restore: Pops the old data segment and registers back into the CPU.
; 5. Return: Uses `iret` to teleport the CPU back to the original code.
isr_common_stub:
    ; 1. Save CPU state
    ; by pushing all the registers
    pusha ; pushes in this order edi,esi,ebp,esp,ebx,edx,ecx,eax
    mov ax, ds  ;ds (Data segment: the specific area of RAM where we store program variables, strings, and global data lives)
                ; when we're looking for a variable ds tells the program which big block of memory to search in.
                ; basically mov ax, ds. is the program trying to note what it's previously doing 
                ; by storing it inside AX because ax is a general purpose register
    
    push eax    ; save the data segment
                ; why eax? because AX is just the bottom half of eax, since eax(extended AX) is 32 bit, and AX is 16 bit
                ; and we put ax (eax) int the stack
    mov ax, 0x10 ; kernel data segment, tells the CPU that we're the kernel and we're entering kernel mode
    mov ds, ax
    mov es, ax ; extra segment it's a backup data signpost used for moving string around
	mov fs, ax 
	mov gs, ax
    
    ; 2. call c isr_handler
    call isr_handler

    ; 3. restore state

    pop eax ; putting the old ds to ax again
    mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	
    popa ; bring bang the edi,esi,ebp,esp,ebx,edx,ecx,eax registers state
    add esp, 8 ; esp (extended stack pointer) 
    sti ; unpause
    iret ; tells the program to return to the previous state before getting interrupted

; IRQ (Interrupt Request)

; @brief Common entry point for all IRQs.
; 
; Workflow:
; 1. Save: Store the User's registers and Data Segment.
; 2. Switch: Change to the Kernel "Uniform" (0x10).
; 3. Execute: Call `irq_handler` in C.
; 4. Restore: Put the User's registers back.
; 5. Return: Teleport back to the interrupted program.
irq_common_stub:
    ; --- 1. SAVE CPU STATE ---
    pusha 

    mov ax, ds
    push eax ; Save the "Old Signpost"

    ; --- 2. ENTER KERNEL MODE ---
    mov ax, 0x10 
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    
    ; --- 3. CALL C HARDWARE HANDLER ---
    ; @extern irq_handler (Defined in irq.c)
    call irq_handler ; <--- This is the "Doctor" for hardware

    ; --- 4. RESTORE STATE ---
    pop eax 
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    
    popa 
    add esp, 8 ; Clean up the stack (Error code + Int number)
    
    ; --- 5. TELEPORT HOME ---
    sti 
    iret

; -----------------------------------------------------------------------------
; SECTION: Internal CPU Exceptions (0-31)
; These occur when the CPU runs into a logical error or a fault.
; -----------------------------------------------------------------------------

; 0 Divide by Zero exception
isr0:
    cli             ; Disable interrupts so we don't get interrupted while handling this
    push byte 0     ; Dummy error code (to keep the stack structure consistent)
    push byte 0     ; The actual interrupt number (0)
    jmp isr_common_stub

; [ ... isr1 through isr7 follow the same pattern ... ]

; 1: Debug Exception
isr1:
    cli
    push byte 0
    push byte 1
    jmp isr_common_stub

; 2: Non Maskable Interrupt Exception
isr2:
    cli
    push byte 0
    push byte 2
    jmp isr_common_stub

; 3: Int 3 Exception
isr3:
    cli
    push byte 0
    push byte 3
    jmp isr_common_stub

; 4: INTO Exception
isr4:
    cli
    push byte 0
    push byte 4
    jmp isr_common_stub

; 5: Out of Bounds Exception
isr5:
    cli
    push byte 0
    push byte 5
    jmp isr_common_stub

; 6: Invalid Opcode Exception
isr6:
    cli
    push byte 0
    push byte 6
    jmp isr_common_stub

; 7: Coprocessor Not Available Exception
isr7:
    cli
    push byte 0
    push byte 7
    jmp isr_common_stub


; @brief Handle "Double Fault" (Interrupt 8)
; @note No dummy push! The CPU hardware automatically pushes an error code for this.
isr8:
    cli
    push byte 8     ; The interrupt number (8)
    jmp isr_common_stub

; 8: Double Fault Exception (With Error Code!)
isr8:
    cli
    push byte 8
    jmp isr_common_stub

; 9: Coprocessor Segment Overrun Exception
isr9:
    cli
    push byte 0
    push byte 9
    jmp isr_common_stub

; 10: Bad TSS Exception (With Error Code!)
isr10:
    cli
    push byte 10
    jmp isr_common_stub

; 11: Segment Not Present Exception (With Error Code!)
isr11:
    cli
    push byte 11
    jmp isr_common_stub

; 12: Stack Fault Exception (With Error Code!)
isr12:
    cli
    push byte 12
    jmp isr_common_stub


; @brief Handle "General Protection Fault" (Interrupt 13)
; @note The CPU provides an error code telling us which segment caused the fault.
isr13:
    cli
    push byte 13    ; The interrupt number (13)
    jmp isr_common_stub

; 14: Page Fault Exception (With Error Code!)
isr14:
    cli
    push byte 14
    jmp isr_common_stub

; 15: Reserved Exception
isr15:
    cli
    push byte 0
    push byte 15
    jmp isr_common_stub

; 16: Floating Point Exception
isr16:
    cli
    push byte 0
    push byte 16
    jmp isr_common_stub

; 17: Alignment Check Exception
isr17:
    cli
    push byte 0
    push byte 17
    jmp isr_common_stub

; 18: Machine Check Exception
isr18:
    cli
    push byte 0
    push byte 18
    jmp isr_common_stub

; 19: Reserved
isr19:
    cli
    push byte 0
    push byte 19
    jmp isr_common_stub

; 20: Reserved
isr20:
    cli
    push byte 0
    push byte 20
    jmp isr_common_stub

; 21: Reserved
isr21:
    cli
    push byte 0
    push byte 21
    jmp isr_common_stub

; 22: Reserved
isr22:
    cli
    push byte 0
    push byte 22
    jmp isr_common_stub

; 23: Reserved
isr23:
    cli
    push byte 0
    push byte 23
    jmp isr_common_stub

; 24: Reserved
isr24:
    cli
    push byte 0
    push byte 24
    jmp isr_common_stub

; 25: Reserved
isr25:
    cli
    push byte 0
    push byte 25
    jmp isr_common_stub

; 26: Reserved
isr26:
    cli
    push byte 0
    push byte 26
    jmp isr_common_stub

; 27: Reserved
isr27:
    cli
    push byte 0
    push byte 27
    jmp isr_common_stub

; 28: Reserved
isr28:
    cli
    push byte 0
    push byte 28
    jmp isr_common_stub

; 29: Reserved
isr29:
    cli
    push byte 0
    push byte 29
    jmp isr_common_stub

; 30: Reserved
isr30:
    cli
    push byte 0
    push byte 30
    jmp isr_common_stub

; 31: Reserved
isr31:
    cli
    push byte 0
    push byte 31
    jmp isr_common_stub

; -----------------------------------------------------------------------------
; SECTION: External Hardware Interrupts (IRQs 0-15 mapped to 32-47)
; These occur when hardware (Keyboard, Timer, Mouse) wants the CPU's attention.
; -----------------------------------------------------------------------------

; @brief System Timer (IRQ 0 -> Interrupt 32)
irq0:
    cli
    push byte 0     ; The original IRQ number (0)
    push byte 32    ; The mapped IDT index (32)
    jmp irq_common_stub

; @brief Keyboard (IRQ 1 -> Interrupt 33)
irq1:
    cli
    push byte 1     ; The original IRQ number (1)
    push byte 33    ; The mapped IDT index (33)
    jmp irq_common_stub

irq2:
	cli
	push byte 2
	push byte 34
	jmp irq_common_stub

irq3:
	cli
	push byte 3
	push byte 35
	jmp irq_common_stub

irq4:
	cli
	push byte 4
	push byte 36
	jmp irq_common_stub

irq5:
	cli
	push byte 5
	push byte 37
	jmp irq_common_stub

irq6:
	cli
	push byte 6
	push byte 38
	jmp irq_common_stub

irq7:
	cli
	push byte 7
	push byte 39
	jmp irq_common_stub

irq8:
	cli
	push byte 8
	push byte 40
	jmp irq_common_stub

irq9:
	cli
	push byte 9
	push byte 41
	jmp irq_common_stub

irq10:
	cli
	push byte 10
	push byte 42
	jmp irq_common_stub

irq11:
	cli
	push byte 11
	push byte 43
	jmp irq_common_stub

irq12:
	cli
	push byte 12
	push byte 44
	jmp irq_common_stub

irq13:
	cli
	push byte 13
	push byte 45
	jmp irq_common_stub

irq14:
	cli
	push byte 14
	push byte 46
	jmp irq_common_stub

irq15:
	cli
	push byte 15
	push byte 47
	jmp irq_common_stub