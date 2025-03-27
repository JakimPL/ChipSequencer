    %include "src/common.asm"

    global _start
    extern initialize
    extern sound_driver_step
    extern sound_driver_terminate
    extern calculate

    %ifdef ELF
    section .text
    %else
    bits 16

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
    call initialize

main_loop:
.check_esc:
    mov ah, BIOS_KEYBOARD_CHECK
    int BIOS_KEYBOARD_INTERRUPT
    jz .no_key

    mov ah, BIOS_KEYBOARD_READ
    int BIOS_KEYBOARD_INTERRUPT
    cmp al, ESC_KEY
    jmp exit

.no_key:
    cmp byte [calculate], 1
    jnz main_loop
    call sound_driver_step
    jmp main_loop

exit:
    call sound_driver_terminate
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
    db 'Chip Sequencer by Jakim, 2025', 13, 10, '$'

    segment stack stack
    resb 0x1000
stack_top:
