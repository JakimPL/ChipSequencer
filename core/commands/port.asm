    SEGMENT_CODE
portamento_up:
    %ifdef USED_COMMAND_PORTAMENTO_UP
    xor eax, eax
    %ifdef TRACKER
    mov al, MAX_SIZE_COMMAND
    %else
    mov al, SIZE_COMMAND_PORTAMENTO_UP
    %endif
    %endif
    ret

portamento_down:
    %ifdef USED_COMMAND_PORTAMENTO_DOWN
    xor eax, eax
    %ifdef TRACKER
    mov al, MAX_SIZE_COMMAND
    %else
    mov al, SIZE_COMMAND_PORTAMENTO_DOWN
    %endif
    %endif
    ret
