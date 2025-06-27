    SEGMENT_CODE
change_float_value:
    %ifdef USED_COMMAND_CHANGE_FLOAT_VALUE
    %define USED_COMMAND_CHANGE_32BIT_VALUE
    call change_32bit_value
    SET_COMMAND_SIZE SIZE_COMMAND_CHANGE_FLOAT_VALUE
    %endif
    ret

change_byte_value:
    %ifdef USED_COMMAND_CHANGE_BYTE_VALUE
    %define USED_COMMAND_LOAD_TARGET
    call load_target
    mov bl, [COMMAND_CHANGE_BYTE_VALUE_VALUE + edi]
    mov [eax], bl
    SET_COMMAND_SIZE SIZE_COMMAND_CHANGE_BYTE_VALUE
    %endif
    ret

change_word_value:
    %ifdef USED_COMMAND_CHANGE_WORD_VALUE
    %define USED_COMMAND_LOAD_TARGET
    call load_target
    mov bx, [COMMAND_CHANGE_WORD_VALUE_VALUE + edi]
    mov [eax], bx
    SET_COMMAND_SIZE SIZE_COMMAND_CHANGE_WORD_VALUE
    %endif
    ret

change_dword_value:
    %ifdef USED_COMMAND_CHANGE_DWORD_VALUE
    %define USED_COMMAND_CHANGE_32BIT_VALUE
    call change_32bit_value
    SET_COMMAND_SIZE SIZE_COMMAND_CHANGE_DWORD_VALUE
    %endif
    ret

add_float_value:
    %ifdef USED_COMMAND_ADD_FLOAT_VALUE
    %define USED_COMMAND_ADD_32BIT_VALUE
    call add_32bit_value
    SET_COMMAND_SIZE SIZE_COMMAND_ADD_FLOAT_VALUE
    %endif
    ret

add_byte_value:
    %ifdef USED_COMMAND_ADD_BYTE_VALUE
    %define USED_COMMAND_LOAD_TARGET
    call load_target
    mov bl, [COMMAND_ADD_BYTE_VALUE_VALUE + edi]
    add [eax], bl
    SET_COMMAND_SIZE SIZE_COMMAND_ADD_BYTE_VALUE
    %endif
    ret

add_word_value:
    %ifdef USED_COMMAND_ADD_WORD_VALUE
    %define USED_COMMAND_LOAD_TARGET
    call load_target
    mov bx, [COMMAND_ADD_WORD_VALUE_VALUE + edi]
    add [eax], bx
    SET_COMMAND_SIZE SIZE_COMMAND_ADD_WORD_VALUE
    %endif
    ret

add_dword_value:
    %ifdef USED_COMMAND_ADD_DWORD_VALUE
    %define USED_COMMAND_ADD_32BIT_VALUE
    call add_32bit_value
    SET_COMMAND_SIZE SIZE_COMMAND_ADD_DWORD_VALUE
    %endif
    ret

    %ifdef USED_COMMAND_CHANGE_32BIT_VALUE
change_32bit_value:
    %define USED_COMMAND_LOAD_TARGET
    call load_target
    mov ebx, [COMMAND_CHANGE_VALUE_VALUE + edi]
    mov [eax], ebx
    ret
    %endif

    %ifdef USED_COMMAND_ADD_32BIT_VALUE
add_32bit_value:
    %define USED_COMMAND_LOAD_TARGET
    call load_target
    mov ebx, [COMMAND_ADD_VALUE_VALUE + edi]
    add [eax], ebx
    ret
    %endif

    %ifdef USED_COMMAND_LOAD_TARGET
load_target:
    movzx eax, word [COMMAND_CHANGE_VALUE_POINTER + edi]
    mov eax, [CDECL(targets) + 4 * eax]
    ret
    %endif
