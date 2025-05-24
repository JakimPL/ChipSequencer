    SEGMENT_CODE
portamento_up:
    %ifdef USED_COMMAND_PORTAMENTO_UP
    %define USED_LOAD_PORTAMENTO_VALUE
    call load_portamenento_value
    fmul
    fistp dword [frequency + 4 * ecx]

    SET_COMMAND_SIZE SIZE_COMMAND_PORTAMENTO_UP
    %endif
    ret

portamento_down:
    %ifdef USED_COMMAND_PORTAMENTO_DOWN
    %define USED_LOAD_PORTAMENTO_VALUE
    call load_portamenento_value
    fxch
    fdiv
    fistp dword [frequency + 4 * ecx]

    SET_COMMAND_SIZE SIZE_COMMAND_PORTAMENTO_DOWN
    %endif
    ret

    %ifdef USED_LOAD_PORTAMENTO_VALUE
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
    %endif
