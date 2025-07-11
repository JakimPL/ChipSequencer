    SEGMENT_CODE
portamento_up:
    %ifdef USED_COMMAND_PORTAMENTO_UP
    %define USED_COMMAND_PORTAMENTO
    call load_portamenento_value
    fmul
    fistp dword [CDECL(frequency) + 4 * ecx]

    SET_COMMAND_SIZE SIZE_COMMAND_PORTAMENTO_UP
    %endif
    ret

portamento_down:
    %ifdef USED_COMMAND_PORTAMENTO_DOWN
    %define USED_COMMAND_PORTAMENTO
    call load_portamenento_value
    fxch
    fdiv
    fistp dword [CDECL(frequency) + 4 * ecx]

    SET_COMMAND_SIZE SIZE_COMMAND_PORTAMENTO_DOWN
    %endif
    ret

    %ifdef USED_COMMAND_PORTAMENTO
load_portamenento_value:
    movzx ecx, byte [COMMAND_PORTAMENTO_CHANNEL + edi]
    movzx eax, word [COMMAND_PORTAMENTO_VALUE + edi]
    mov ebx, [CDECL(frequency) + 4 * ecx]

    push eax
    fild dword [esp]
    pop eax

    fidiv word [i_1000]
    fld qword [CDECL(note_divisor)]

    fyl2x
    call power
    fild dword [CDECL(frequency) + 4 * ecx]
    ret
    %endif
