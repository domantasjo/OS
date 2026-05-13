[org 0x7C00] ;Bootloader is loaded at this address
mov bp, 0x8000 ;Set up stack
mov sp, bp

mov bx, 0x1000 ; Load kernel at 0x1000
xor ax, ax
mov ds, ax
mov es, ax     ; Set ES to 0
mov dh, 32     ; Read 32 sectors (16 KB) - grow as the kernel grows
; the bios sets 'dl' for our boot disk number

call disk_load

mov bx, MSG_LOADED
call print
call print_nl
call do_e820

; Switch to 32-bit protected mode
cli            ; Disable interrupts

; Enable the A20 line via the fast A20 gate (port 0x92)
in  al, 0x92
or  al, 2
out 0x92, al

lgdt [gdt_descriptor]

mov eax, cr0
or  eax, 0x1   ; Set PE (Protection Enable) bit
mov cr0, eax

jmp CODE_SEG:init_pm ; Far jump to flush pipeline and enter 32-bit mode

%include "print.asm"
%include "disk.asm"
%include "memory.asm"

; ── GDT ─────────────────────────────────────────────────────────────────────
gdt_start:
    ; Null descriptor (required)
    dd 0x0
    dd 0x0

gdt_code:          ; Code segment: base=0, limit=0xFFFFF, 32-bit, execute/read
    dw 0xFFFF      ; Limit (bits 0-15)
    dw 0x0         ; Base  (bits 0-15)
    db 0x0         ; Base  (bits 16-23)
    db 10011010b   ; Access byte: present, ring0, code, execute/read
    db 11001111b   ; Flags (4-bit) + Limit (bits 16-19): 4KB granularity, 32-bit
    db 0x0         ; Base  (bits 24-31)

gdt_data:          ; Data segment: base=0, limit=0xFFFFF, 32-bit, read/write
    dw 0xFFFF
    dw 0x0
    db 0x0
    db 10010010b   ; Access byte: present, ring0, data, read/write
    db 11001111b
    db 0x0

gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1  ; GDT size minus 1
    dd gdt_start                 ; GDT address

CODE_SEG equ gdt_code - gdt_start
DATA_SEG equ gdt_data - gdt_start

; ── 32-bit protected mode entry ─────────────────────────────────────────────
[bits 32]
init_pm:
    mov ax, DATA_SEG  ; Reload all segment registers with data descriptor
    mov ds, ax
    mov ss, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    mov ebp, 0x90000  ; Set up a new stack in protected mode
    mov esp, ebp

    jmp 0x1000        ; Jump to the kernel

MSG_LOADED: db 'Kernel loaded, entering PM...', 0

times(510 - ($ - $$)) db 0 ;Magic number
dw 0xAA55 ;Boot signature
