    SEGMENT_CODE
set_bpm:
    xor eax, eax
    %ifdef TRACKER
    mov al, MAX_SIZE_COMMAND
    %else
    mov al, SIZE_COMMAND_SET_BPM
    %endif
    ret
