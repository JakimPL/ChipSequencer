    %define DIRECT_MODE 1
    %define OUTPUT_CHANNELS 2
    %define DSP_BUFFER_SIZE 0x1000

    %define BIOS_KEYBOARD_INTERRUPT 0x16
    %define BIOS_KEYBOARD_CHECK 0x01
    %define BIOS_KEYBOARD_READ 0x00
    %define ESC_KEY 0x1B

    %define DOS_INTERRUPT 0x21
    %define DOS_TERMINATE 0x4C
    %define DOS_PRINT_STRING 0x09
    %define DOS_ALLOCATE_MEMORY 0x48

    bits 32
    org 0x0100

    section .text
    global start
start:
; call allocate_memory_for_buffers
    call initialize
    call initialize_frequencies
    call reset_channels
    call reset_dsps
    call calculate_ticks_per_beat
    call generate_sine_table

    mov ah, DOS_TERMINATE
    int DOS_INTERRUPT

main_loop:
.check_esc:
    mov ah, BIOS_KEYBOARD_CHECK
    int BIOS_KEYBOARD_INTERRUPT
    jz .no_key

    mov ah, BIOS_KEYBOARD_READ
    int BIOS_KEYBOARD_INTERRUPT
    cmp al, ESC_KEY
    je .exit

.no_key:
    cmp byte [calculate], 1
    jnz main_loop
    call sound_driver_step
    jmp main_loop

.exit:
    call terminate

.return_to_dos:
    mov ah, DOS_TERMINATE
    int DOS_INTERRUPT

allocate_memory_for_buffers:
    mov ah, DOS_ALLOCATE_MEMORY
    mov bx, DSP_BUFFER_SIZE / 16
    int DOS_INTERRUPT
    jc .allocation_error
    ret
.allocation_error:
    mov ah, DOS_PRINT_STRING
    mov dx, allocation_error_message
    int DOS_INTERRUPT
    ret

    %include "SRC\CONST.ASM"
    %include "SRC\UTILS.ASM"
    %include "SRC\SONG.ASM"
    %include "SRC\ELEMENTS.ASM"

    %if DIRECT_MODE
    %include "SRC\DRIVER\DIRECT.ASM"
    %else
    %include "SRC\DRIVER\SB16.ASM"
    %endif

    section .data
calculate:
    db 1
dividend:
    dd 0x71AE0000
dsp_buffer_pointer:
    dw 0
allocation_error_message:
    db 'Memory allocation error$', 0

    section .bss
    output resd OUTPUT_CHANNELS

; Buffers
    dsp_buffer resd DSP_BUFFER_SIZE * DSPS
