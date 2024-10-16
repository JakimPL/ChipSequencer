    %define DIRECT_MODE 1
    %define PRECALCULATE 1

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

    %macro SEGMENT_BUFFERS 0
    %if EXE
    segment buffer
    %else
    section .buffer
    %endif
    %endmacro

    %macro LOAD_FUNCTION 2
    call [ds:%1 + %2]
    %endmacro

    %macro PRINT_STRING 1
    mov dx, %1
    call print_message
    %endmacro

    %if EXE
    bits 32
    segment code
    global start
    %else
    bits 16
    org 0x0100
    section .text
    %endif

start:
    %if EXE
    call prepare_stack
    %if PRECALCULATE
    call allocate_memory
    %endif
    %endif

    PRINT_STRING message

    call initialize_frequencies
    call calculate_ticks_per_beat
    call generate_sine_table

    call reset_channels

    %if PRECALCULATE & EXE
.precalculate:
    call precalculate
    %endif

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
    call terminate
    call return_to_dos

    %include "SRC\CONST.ASM"
    %include "SRC\SONG.ASM"
    %include "SRC\UTILS.ASM"
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

    %if EXE
    segment stack stack
    resb 0x1000
stacktop:
    %endif

    SEGMENT_BSS
    output resd OUTPUT_CHANNELS

