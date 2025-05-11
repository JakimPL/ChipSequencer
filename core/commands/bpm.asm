    SEGMENT_CODE
set_bpm:
    %ifdef USED_COMMAND_SET_BPM
    mov ax, word [COMMAND_SET_BPM_BPM + edi]
    mov [bpm], ax
    call calculate_ticks_per_beat

    xor eax, eax
    %ifdef TRACKER
    mov al, MAX_SIZE_COMMAND
    %else
    mov al, SIZE_COMMAND_SET_BPM
    %endif
    %endif
    ret
