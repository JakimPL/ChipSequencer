    SEGMENT_CODE
portamento_up:
    xor eax, eax
    %ifdef TRACKER
    mov al, MAX_SIZE_COMMAND
    %else
    mov al, SIZE_COMMAND_PORTAMENTO_UP
    %endif
    ret

portamento_down:
    xor eax, eax
    %ifdef TRACKER
    mov al, MAX_SIZE_COMMAND
    %else
    mov al, SIZE_COMMAND_PORTAMENTO_DOWN
    %endif
    ret
