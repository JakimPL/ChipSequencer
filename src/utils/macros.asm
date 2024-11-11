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
    call [ds:%1 + %2]
    %endmacro

    %macro PRINT_STRING 1
    mov dx, %1
    call print_message
    %endmacro

    %macro LOAD_OFFSET 2
    %ifdef ELF
    mov %1, %2
    %else
    movzx %1, word [%2]
    %endif
    %endmacro
