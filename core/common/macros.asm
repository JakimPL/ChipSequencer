    %ifdef WIN32
    %define CDECL(x) _ %+ x
    %else
    %define CDECL(x) x
    %endif

    %macro SEGMENT_CODE 0
    section .text
    %endmacro

    %macro SEGMENT_DATA 0
    section .data
    %endmacro

    %macro SEGMENT_BSS 0
    section .bss
    %endmacro

    %macro LOAD_FUNCTION 2
    call [ds:%1 + 4 * %2]
    %endmacro

    %macro LOAD_ITEM 2
    mov ecx, [%1]
    mov ecx, [ecx + 4 * ebx]
    mov [%2], ecx
    %endmacro

    %macro LOAD_ARRAY_ITEM 3
    %ifdef TRACKER
    LOAD_ITEM %1, %2
    %else
    imul ebx, %3
    lea ecx, [%1 + ebx]
    mov [%2], ecx
    %endif
    %endmacro

    %macro LOAD_VECTOR_ITEM 2
    %ifdef TRACKER
    LOAD_ITEM %1, %2
    %else
    lea ecx, [%1]
    call load_item
    mov [%2], ecx
    %endif
    %endmacro

    %macro LOAD_VECTOR_ITEM_16_BIT 2
    %ifdef TRACKER
    LOAD_ITEM %1, %2
    %else
    lea ecx, [%1]
    call load_item_16bit
    mov [%2], ecx
    %endif
    %endmacro

    %macro PRINT_STRING 1
    mov dx, %1
    call print_message
    %endmacro

    %macro LOAD_OFFSET 2
    mov %1, [%2]
    %endmacro

    %macro MOV_FROM_DI 1
    mov %1, [edi]
    %endmacro

    %macro MOV_TO_DI 1
    mov [edi], %1
    %endmacro

    %macro ADD_TO_DI 1
    add [edi], %1
    %endmacro

    %macro MOV_FROM_SI 1
    mov %1, [esi]
    %endmacro

    %macro MOV_TO_SI 1
    mov [esi], %1
    %endmacro

    %macro ADD_TO_SI 1
    add [esi], %1
    %endmacro

    %macro SET_COMMAND_SIZE 1
    xor eax, eax
    %ifdef TRACKER
    mov al, MAX_SIZE_COMMAND
    %else
    mov al, %1
    %endif
    %endmacro
