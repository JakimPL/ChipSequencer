    SEGMENT_CODE
portamento_up:
    %ifdef USED_COMMAND_PORTAMENTO_UP
    call load_portamenento_value
    fmulp st1, st0
    fistp dword [frequency + 4 * ecx]

    xor eax, eax
    %ifdef TRACKER
    mov al, MAX_SIZE_COMMAND
    %else
    mov al, SIZE_COMMAND_PORTAMENTO_UP
    %endif
    %endif
    ret

portamento_down:
    %ifdef USED_COMMAND_PORTAMENTO_DOWN
    call load_portamenento_value
    fdivp st1, st0
    fistp dword [frequency + 4 * ecx]

    xor eax, eax
    %ifdef TRACKER
    mov al, MAX_SIZE_COMMAND
    %else
    mov al, SIZE_COMMAND_PORTAMENTO_DOWN
    %endif
    %endif
    ret

load_portamenento_value:
    movzx ecx, byte [COMMAND_PORTAMENTO_CHANNEL + edi]
    movzx eax, word [COMMAND_PORTAMENTO_VALUE + edi]
    mov ebx, [frequency + 4 * ecx]

    push eax
    fild dword [esp]
    pop eax

    fidiv word [i_1000]
    fld qword [note_divisor]

    fyl2x
    call power
    fild dword [frequency + 4 * ecx]
    ret
