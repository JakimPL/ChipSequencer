    %include "SRC\CONFIG.ASM"
    %include "SRC\CONST.ASM"
    %include "SRC\UTILS\MACROS.ASM"

    bits 16

    %if EXE
    segment code use16
    global _start
    extern player

    segment data use16
    segment bss use16
    segment code
    %else
    org 0x0100
    section .text
    %endif

    SEGMENT_CODE
_start:
    %if EXE
.prepare_stack:
    mov ax, data
    mov ds, ax
    mov ax, stack
    mov ss, ax
    mov sp, stack_top
    %endif

    call player

    %if EXE
    segment stack stack
    resb 0x1000
stack_top:
    %endif
