; Remap the 8259A PIC so that hardware IRQs don't overlap CPU exception vectors.
; By default the BIOS maps IRQ 0-7 to vectors 8-15, which clash with CPU exceptions.
; We move them to vectors 32-47 (0x20-0x2F), which are free for hardware use.
global map_pic

; ICW1 (Initialization Control Word 1) -- sent to the control port to start init.
; 0x11 = 00010001b:
;   bit 0 = 1 -> we will send ICW4 after this
;   bit 3 = 0 -> edge triggered mode
;   bit 4 = 1 -> start initialization sequence
%define ICW_1 0x11

%define PIC_1_CTRL 0x20 ; Master PIC: writing here sends commands (ICW1, OCW2 EOI)
%define PIC_2_CTRL 0xA0 ; Slave  PIC: same

%define PIC_1_DATA 0x21 ; Master PIC: writing here sends data (ICW2/3/4, OCW1 mask)
%define PIC_2_DATA 0xA1 ; Slave  PIC: same

%define IRQ_0 0x20      ; Master PIC IRQs 0-7  -> vectors 0x20-0x27 (32-39)
%define IRQ_8 0x28      ; Slave  PIC IRQs 8-15 -> vectors 0x28-0x2F (40-47)


map_pic:
    ; Disable interrupts for the duration of the reprogramming sequence.
    ; If a hardware IRQ fires while the PIC is halfway through initialization
    ; it will end up in an undefined state and likely triple-fault the CPU.
    cli

    ; ---- ICW1: begin initialization ----------------------------------------
    ; Sending ICW1 to the control port tells both PICs to expect ICW2, ICW3,
    ; and ICW4 to follow on their data ports.
    mov al, ICW_1
    out PIC_1_CTRL, al  ; start init on master
    out PIC_2_CTRL, al  ; start init on slave

    ; ---- ICW2: vector offset ------------------------------------------------
    ; Tells each PIC which IDT vector its IRQ 0 maps to.
    ; Master: IRQ 0 (timer) -> vector 32 (0x20), IRQ 1 -> 33, ... IRQ 7 -> 39
    ; Slave:  IRQ 8         -> vector 40 (0x28), IRQ 9 -> 41, ... IRQ 15 -> 47
    mov al, IRQ_0
    out PIC_1_DATA, al

    mov al, IRQ_8
    out PIC_2_DATA, al

    ; ---- ICW3: cascade wiring -----------------------------------------------
    ; Tells the master which IRQ line the slave is attached to,
    ; and tells the slave its own cascade identity number.
    ; The slave is wired to IRQ 2 on the master, so master gets 0x04 (bit 2 set).
    ; The slave gets its cascade identity = 2.
    mov al, 0x4
    out PIC_1_DATA, al  ; master: slave is on IRQ line 2 (0000 0100)

    mov al, 0x2
    out PIC_2_DATA, al  ; slave: my cascade identity is 2

    ; ---- ICW4: environment info ---------------------------------------------
    ; 0x01 = 00000001b: bit 0 = 1 -> 8086/88 mode (as opposed to MCS-80/85).
    ; Must be sent because we set bit 0 of ICW1 earlier.
    mov al, 1
    out PIC_1_DATA, al
    out PIC_2_DATA, al

    ; ---- OCW1: interrupt mask register (IMR) --------------------------------
    ; Each bit masks (blocks) the corresponding IRQ: 1 = masked, 0 = unmasked.
    ; 0xFF masks all 16 IRQs so nothing fires until we're ready to handle it.
    ; Unmask individual IRQs only after registering their IDT handlers.
    mov al, 0xFF
    out PIC_1_DATA, al  ; mask all master IRQs (0-7)
    out PIC_2_DATA, al  ; mask all slave  IRQs (8-15)

    ; Re-enable interrupts now that the PIC is safely reconfigured.
    sti
    ret
