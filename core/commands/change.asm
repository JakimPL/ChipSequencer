    SEGMENT_CODE
change_byte_value:
    xor eax, eax
    %ifdef TRACKER
    mov al, MAX_SIZE_COMMAND
    %else
    mov al, SIZE_COMMAND_CHANGE_BYTE_VALUE
    %endif
    ret

change_word_value:
    xor eax, eax
    %ifdef TRACKER
    mov al, MAX_SIZE_COMMAND
    %else
    mov al, SIZE_COMMAND_CHANGE_WORD_VALUE
    %endif
    ret

change_dword_value:
    xor eax, eax
    %ifdef TRACKER
    mov al, MAX_SIZE_COMMAND
    %else
    mov al, SIZE_COMMAND_CHANGE_DWORD_VALUE
    %endif
    ret

change_float_value:
    xor eax, eax
    %ifdef TRACKER
    mov al, MAX_SIZE_COMMAND
    %else
    mov al, SIZE_COMMAND_CHANGE_FLOAT_VALUE
    %endif
    ret
