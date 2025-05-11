    SEGMENT_CODE
set_master_gainer:
    %ifdef USED_COMMAND_SET_MASTER_GAINER
    movzx eax, word [COMMAND_SET_MASTER_GAINER_GAIN + edi]
    push eax
    fild dword [esp]
    pop eax
    fdiv dword [half_range]
    fstp dword [normalizer]

    xor eax, eax
    %ifdef TRACKER
    mov al, MAX_SIZE_COMMAND
    %else
    mov al, SIZE_COMMAND_SET_MASTER_GAINER
    %endif
    %endif
    ret
