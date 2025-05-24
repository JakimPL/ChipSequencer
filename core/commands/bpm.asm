    SEGMENT_CODE
set_bpm:
    %ifdef USED_COMMAND_SET_BPM
    mov ax, word [COMMAND_SET_BPM_BPM + edi]
    mov [bpm], ax
    call calculate_ticks_per_beat

    SET_COMMAND_SIZE SIZE_COMMAND_SET_BPM
    %endif
    ret
