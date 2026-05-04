print:
    pusha
.loop:
    mov ah, 0x0E ;Teletype output
    mov al, [bx]
    cmp al, 0
    je done
    int 0x10
    add bx, 1
    jmp .loop

done:
    popa
    ret

print_nl:
    pusha
    mov ah, 0x0E
    mov al, 13          ; carriage return
    int 0x10
    mov al, 10          ; newline
    int 0x10
    popa
    ret

print_hex:
    pusha
    mov cx, 0

hex_loop:
    cmp cx, 4
    je done_hex
    mov ax, dx
    and ax, 0x000f
    add ax, 0x30
    cmp ax, 0x39
    jle step2
    add al, 7 ; Convert to uppercase letter for hex digits A-F

step2:
    mov bx, HEX_OUT + 5
    sub bx, cx          ; write right-to-left: pos 5, 4, 3, 2
    mov [bx], al
    ror dx, 4 ; Rotate right to get the next hex digit 0x1234 -> 0x4123 -> 0x3412 -> 0x2341 -> 0x1234
    add cx, 1
    jmp hex_loop

done_hex:
    mov bx, HEX_OUT
    call print
    popa
    ret
    
HEX_OUT: db '0x0000', 0 ; Reserving memory for our output