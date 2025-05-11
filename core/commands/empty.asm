    SEGMENT_CODE
empty:
    xor eax, eax
    %ifdef TRACKER
    mov al, MAX_SIZE_COMMAND
    %else
    mov al, SIZE_COMMAND
    %endif
    ret
