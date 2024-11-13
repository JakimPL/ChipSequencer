    %macro SEGMENT_CODE 0
    %ifdef ELF
    section .text
    %else
    segment code
    %endif
    %endmacro

    %macro SEGMENT_DATA 0
    %ifdef ELF
    section .data
    %else
    segment data
    %endif
    %endmacro

    %macro SEGMENT_BSS 0
    %ifdef ELF
    section .bss
    %else
    segment bss
    %endif
    %endmacro

    %macro LOAD_FUNCTION 2
    %ifdef ELF
    call [ds:%1 + 4 * %2]
    %else
    call [ds:%1 + 2 * %2]
    %endif
    %endmacro

    %macro PRINT_STRING 1
    mov dx, %1
    call print_message
    %endmacro

    %macro LOAD_OFFSET 2
    %ifdef ELF
    mov %1, [%2]
    %else
    movzx %1, word [%2]
    %endif
    %endmacro

    %macro MOV_FROM_DI 1
    %ifdef ELF
    mov %1, [edi]
    %else
    mov %1, [di]
    %endif
    %endmacro

    %macro MOV_TO_DI 1
    %ifdef ELF
    mov [edi], %1
    %else
    mov [di], %1
    %endif
    %endmacro

    %macro ADD_TO_DI 1
    %ifdef ELF
    add [edi], %1
    %else
    add [di], %1
    %endif
    %endmacro

    %macro MOV_FROM_SI 1
    %ifdef ELF
    mov %1, [esi]
    %else
    mov %1, [si]
    %endif
    %endmacro

    %macro MOV_TO_SI 1
    %ifdef ELF
    mov [esi], %1
    %else
    mov [si], %1
    %endif
    %endmacro

    %macro ADD_TO_SI 1
    %ifdef ELF
    add [esi], %1
    %else
    add [si], %1
    %endif
    %endmacro
