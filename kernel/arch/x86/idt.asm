extern interrupt_handler
extern save_current_esp
extern get_next_esp
extern schedule


%macro isr_err_stub 1
isr_stub_%+%1:
    push dword %1
    jmp isr_common
%endmacro

%macro isr_no_err_stub 1
isr_stub_%+%1:
    push dword 0
    push dword %1
    jmp isr_common
%endmacro

isr_common:
    pusha                      ; save eax,ecx,edx,ebx,esp,ebp,esi,edi (32 bytes)
    push dword [esp + 36]      ; err    (was at [esp+4]; now at [esp+36] after pusha)
    push dword [esp + 36]      ; vector (was at [esp]; now at [esp+32]; +4 from first push)
    call interrupt_handler
    add esp, 8

    mov eax, [esp + 32]
    cmp eax, 32
    jne .no_switch

    ; save esp directly into processes[current_process].esp
    ; we need a C function that takes no args and saves esp internally
    mov ebx, esp
    push ebx
    call save_current_esp
    add esp, 4

    call schedule

    call get_next_esp
    mov esp, eax

    .no_switch:
    popa
    add esp, 8
    iret
isr_no_err_stub 0
isr_no_err_stub 1
isr_no_err_stub 2
isr_no_err_stub 3
isr_no_err_stub 4
isr_no_err_stub 5
isr_no_err_stub 6
isr_no_err_stub 7
isr_err_stub    8
isr_no_err_stub 9
isr_err_stub    10
isr_err_stub    11
isr_err_stub    12
isr_err_stub    13
isr_err_stub    14
isr_no_err_stub 15
isr_no_err_stub 16
isr_err_stub    17
isr_no_err_stub 18
isr_no_err_stub 19
isr_no_err_stub 20
isr_no_err_stub 21
isr_no_err_stub 22
isr_no_err_stub 23
isr_no_err_stub 24
isr_no_err_stub 25
isr_no_err_stub 26
isr_no_err_stub 27
isr_no_err_stub 28
isr_no_err_stub 29
isr_err_stub    30
isr_no_err_stub 31
isr_no_err_stub 32
isr_no_err_stub 33
isr_no_err_stub 34
isr_no_err_stub 35
isr_no_err_stub 36
isr_no_err_stub 37
isr_no_err_stub 38
isr_no_err_stub 39
isr_no_err_stub 40
isr_no_err_stub 41
isr_no_err_stub 42
isr_no_err_stub 43
isr_no_err_stub 44
isr_no_err_stub 45
isr_no_err_stub 46
isr_no_err_stub 47

global isr_stub_table
isr_stub_table:
%assign i 0
%rep 48
    dd isr_stub_%+i
%assign i i+1
%endrep
