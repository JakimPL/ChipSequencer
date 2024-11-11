    %include "src/config.asm"
    %include "src/const.asm"
    %include "src/utils/macros.asm"

    %ifdef ELF
    global _start
    extern player

    section .text
    %else
    bits 16
    global _start
    extern player

    segment code use16
    segment data use16
    segment bss use16
    segment code
    %endif

_start:
.prepare_stack:
    xor edi, edi
    xor esi, esi
    mov ax, data
    mov ds, ax
    mov ax, stack
    mov ss, ax
    mov sp, stack_top

    PRINT_STRING message
    call player
    call return_to_dos

return_to_dos:
    %if PRECALCULATE
    call deallocate_memory
    %endif
    mov ah, DOS_TERMINATE
    int DOS_INTERRUPT

print_message:
; DX - message address
    mov ah, DOS_PRINT_STRING
    int DOS_INTERRUPT
    ret

    SEGMENT_DATA
message:
    db 'Chip Sequencer by Jakim, 2024', 13, 10, '$'

    segment stack stack
    resb 0x1000
stack_top:
