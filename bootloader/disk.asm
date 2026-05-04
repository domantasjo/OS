; load 'dh' sectors from drive 'dl' into ES:BX
disk_load:
    pusha

    push dx

    mov ah, 0x02; int 0x13 interrupt, 0x02 is the function to read sectors
    mov al, dh ; Number of sectors to read
    mov cl, 0x02 ; Starting sector (1-based)
    mov ch, 0x00 ; Cylinder number
    mov dh, 0x00 ; Head number
    int 0x13 ; Call BIOS disk service
    jc disk_error ; Jump if carry flag is set (error)
    pop dx
    cmp al,dh ; Check if the number of sectors read matches the requested number
    jne sectors_error
    popa
    ret

disk_error:
    mov bx, DISK_ERROR
    call print
    call print_nl
    mov dh, ah ;ah= error code, dl = disk drive that caused the error
    jmp disk_loop

sectors_error:
    mov bx, SECTORS_ERROR
    call print
    call print_nl

disk_loop:
    jmp $


DISK_ERROR: db 'Disk read error', 0
SECTORS_ERROR: db 'Sector count mismatch', 0