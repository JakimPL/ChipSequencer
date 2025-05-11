    SEGMENT_CODE
change_byte_value:
    xor eax, eax
    mov al, SIZE_COMMAND_CHANGE_BYTE_VALUE
    ret

change_word_value:
    xor eax, eax
    mov al, SIZE_COMMAND_CHANGE_WORD_VALUE
    ret

change_dword_value:
    xor eax, eax
    mov al, SIZE_COMMAND_CHANGE_DWORD_VALUE
    ret

change_float_value:
    xor eax, eax
    mov al, SIZE_COMMAND_CHANGE_FLOAT_VALUE
    ret
