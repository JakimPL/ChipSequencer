    SEGMENT_CODE
change_byte_value:
    %ifdef USED_COMMAND_CHANGE_BYTE_VALUE
    xor eax, eax
    %ifdef TRACKER
    mov al, MAX_SIZE_COMMAND
    %else
    mov al, SIZE_COMMAND_CHANGE_BYTE_VALUE
    %endif
    %endif
    ret

change_word_value:
    %ifdef USED_COMMAND_CHANGE_WORD_VALUE
    xor eax, eax
    %ifdef TRACKER
    mov al, MAX_SIZE_COMMAND
    %else
    mov al, SIZE_COMMAND_CHANGE_WORD_VALUE
    %endif
    %endif
    ret

change_dword_value:
    %ifdef USED_COMMAND_CHANGE_DWORD_VALUE
    xor eax, eax
    %ifdef TRACKER
    mov al, MAX_SIZE_COMMAND
    %else
    mov al, SIZE_COMMAND_CHANGE_DWORD_VALUE
    %endif
    %endif
    ret

change_float_value:
    %ifdef USED_COMMAND_CHANGE_FLOAT_VALUE
    xor eax, eax
    %ifdef TRACKER
    mov al, MAX_SIZE_COMMAND
    %else
    mov al, SIZE_COMMAND_CHANGE_FLOAT_VALUE
    %endif
    %endif
    ret
