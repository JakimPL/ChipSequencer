    %macro SEGMENT_CODE 0
    %if EXE
    segment code
    %else
    section .text
    %endif
    %endmacro

    %macro SEGMENT_DATA 0
    %if EXE
    segment data
    %else
    section .data
    %endif
    %endmacro

    %macro SEGMENT_BSS 0
    %if EXE
; TODO: Fix overlapping memory
    segment data
    %else
    section .bss
    %endif
    %endmacro

    %macro LOAD_FUNCTION 2
    call [ds:%1 + %2]
    %endmacro

    %macro PRINT_STRING 1
    mov dx, %1
    call print_message
    %endmacro
