    SEGMENT_CODE
set_division:
    xor eax, eax
    %ifdef TRACKER
    mov al, MAX_SIZE_COMMAND
    %else
    mov al, SIZE_COMMAND_SET_DIVISION
    %endif
    ret
