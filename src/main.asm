    %define DIRECT_MODE 1
    %define OUTPUT_CHANNELS 2
    %define DSP_BUFFER_SIZE 0x0040

    %define BIOS_KEYBOARD_INTERRUPT 0x16
    %define BIOS_KEYBOARD_CHECK 0x01
    %define BIOS_KEYBOARD_READ 0x00
    %define ESC_KEY 0x1B

    %define DOS_INTERRUPT 0x21
    %define DOS_TERMINATE 0x4C
    %define DOS_PRINT_STRING 0x09

    %macro SEGMENT_CODE 0
    %ifdef EXE
    segment code
    %else
    section .text
    %endif
    %endmacro

    %macro SEGMENT_DATA 0
    %ifdef EXE
    segment data
    %else
    section .data
    %endif
    %endmacro

    %macro SEGMENT_BSS 0
    %ifdef EXE
    segment .bss
    %else
    section .bss
    %endif
    %endmacro

    SEGMENT_CODE

    %ifdef EXE
    bits 32
    global start
    %else
    bits 16
    org 0x0100
    %endif

start:
    %ifdef EXE
    call prepare_stack
    %endif
    call initialize
    call initialize_frequencies
    call reset_channels
    call calculate_ticks_per_beat
    call generate_sine_table

main_loop:
    call print_message
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
; call sound_driver_step
    jmp main_loop

exit:
    call terminate

.return_to_dos:
    mov ah, DOS_TERMINATE
    int DOS_INTERRUPT

print_message:
    mov ah, DOS_PRINT_STRING
    mov dx, message
    int DOS_INTERRUPT
    ret

    %ifdef EXE
prepare_stack:
    mov ax, data
    mov ds, ax
    mov ax, stack
    mov ss, ax
    mov esp, stacktop
    ret
    %endif

    %include "SRC\CONST.ASM"
    %include "SRC\UTILS.ASM"
    %include "SRC\SONG.ASM"
    %include "SRC\ELEMENTS.ASM"

    %if DIRECT_MODE
    %include "SRC\DRIVER\DIRECT.ASM"
    %else
    %include "SRC\DRIVER\SB16.ASM"
    %endif

    SEGMENT_DATA
calculate:
    db 1
dividend:
    dd 0x71AE0000
message:
    db 'Chip Sequencer by Jakim, 2024', 13, 10, '$'

    %ifdef EXE
    segment stack stack
    resb 0x1000
stacktop:
    %endif

    SEGMENT_BSS
    output resd OUTPUT_CHANNELS
