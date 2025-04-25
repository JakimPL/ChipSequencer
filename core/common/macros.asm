    %macro SEGMENT_CODE 0
    %ifndef BITS_16
    section .text
    %else
    segment code
    %endif
    %endmacro

    %macro SEGMENT_DATA 0
    %ifndef BITS_16
    section .data
    %else
    segment data
    %endif
    %endmacro

    %macro SEGMENT_BSS 0
    %ifndef BITS_16
    section .bss
    %else
    segment bss
    %endif
    %endmacro

    %macro LOAD_FUNCTION 2
    %ifndef BITS_16
    call [ds:%1 + 4 * %2]
    %else
    call [ds:%1 + 2 * %2]
    %endif
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

    %macro PRINT_STRING 1
    mov dx, %1
    call print_message
    %endmacro

    %macro LOAD_OFFSET 2
    %ifndef BITS_16
    mov %1, [%2]
    %else
    movzx %1, word [%2]
    %endif
    %endmacro

    %macro MOV_FROM_DI 1
    %ifndef BITS_16
    mov %1, [edi]
    %else
    mov %1, [di]
    %endif
    %endmacro

    %macro MOV_TO_DI 1
    %ifndef BITS_16
    mov [edi], %1
    %else
    mov [di], %1
    %endif
    %endmacro

    %macro ADD_TO_DI 1
    %ifndef BITS_16
    add [edi], %1
    %else
    add [di], %1
    %endif
    %endmacro

    %macro MOV_FROM_SI 1
    %ifndef BITS_16
    mov %1, [esi]
    %else
    mov %1, [si]
    %endif
    %endmacro

    %macro MOV_TO_SI 1
    %ifndef BITS_16
    mov [esi], %1
    %else
    mov [si], %1
    %endif
    %endmacro

    %macro ADD_TO_SI 1
    %ifndef BITS_16
    add [esi], %1
    %else
    add [si], %1
    %endif
    %endmacro
