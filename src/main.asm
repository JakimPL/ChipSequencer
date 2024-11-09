    %include "SRC\CONFIG.ASM"
    %include "SRC\CONST.ASM"
    %include "SRC\UTILS\MACROS.ASM"

    bits 16

    segment code use16
    global _start
    extern player

    segment data use16
    segment bss use16
    segment code
_start:
.prepare_stack:
    mov ax, data
    mov ds, ax
    mov ax, stack
    mov ss, ax
    mov sp, stack_top

    call player

    segment stack stack
    resb 0x1000
stack_top:
