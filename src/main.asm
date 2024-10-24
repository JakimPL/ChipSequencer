    %define DIRECT_MODE 0
    %define PRECALCULATE 0
    %define TABLE_SIZE 0x1000
    %define SAMPLE_RATE 0x7800
    %define SB_BUFFER_SIZE 0x2000

    %include "SRC\CONST.ASM"
    %include "SRC\UTILS\MACROS.ASM"

    bits 16

    %if EXE
    segment code use16
    global start

    segment data use16
    segment bss use16
    %ifn DIRECT_MODE
    buffer resb SB_BUFFER_SIZE
    %endif
    segment code
    %else
    org 0x0100
    %ifn DIRECT_MODE
    section .bss
    buffer resb SB_BUFFER_SIZE
    %endif
    section .text
    %endif

start:
    %if EXE
.prepare_stack:
    mov ax, data
    mov ds, ax
    mov ax, stack
    mov ss, ax
    mov sp, stacktop
    %endif

.initialize:
    PRINT_STRING message

    call initialize_frequencies
    call calculate_ticks_per_beat

    call initialize_dsp_buffers
    call reset_channels
    call reset_dsps

    call generate_sine_table
    call generate_wavetables

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
    dd SAMPLE_RATE << 16
message:
    db 'Chip Sequencer by Jakim, 2024', 13, 10, '$'

    %if EXE
    segment stack stack
    resb 0x1000
stacktop:
    %endif

    SEGMENT_BSS
    output resd OUTPUT_CHANNELS
    dsp_buffer resd DSP_BUFFER_SIZE

    %if EXE
    group dgroup bss data
    %endif
