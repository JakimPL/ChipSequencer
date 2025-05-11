    SEGMENT_CODE
set_division:
    %ifdef USED_COMMAND_SET_DIVISION
    fild word [i_240]
    movzx eax, byte [COMMAND_SET_DIVISION_DIVISION + edi]
    push eax
    fidiv dword [esp]
    pop eax

    fstp dword [unit]
    call calculate_ticks_per_beat

    xor eax, eax
    %ifdef TRACKER
    mov al, MAX_SIZE_COMMAND
    %else
    mov al, SIZE_COMMAND_SET_DIVISION
    %endif
    %endif
    ret
