    SEGMENT_CODE
set_bpm:
    mov ax, word [COMMAND_SET_BPM_BPM + edi]
    mov [bpm], ax
    call calculate_ticks_per_beat

    %ifdef USED_COMMAND_SET_BPM
    xor eax, eax
    %ifdef TRACKER
    mov al, MAX_SIZE_COMMAND
    %else
    mov al, SIZE_COMMAND_SET_BPM
    %endif
    %endif
    ret
