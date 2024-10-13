    %define DIRECT_MODE 1
    %define OUTPUT_CHANNELS 2
    %define DSP_BUFFER_SIZE 0x0100

    %define BIOS_KEYBOARD_INTERRUPT 0x16
    %define BIOS_KEYBOARD_CHECK 0x01
    %define BIOS_KEYBOARD_READ 0x00
    %define ESC_KEY 0x1B

    %define DOS_INTERRUPT 0x21
    %define DOS_TERMINATE 0x4C
    %define DOS_PRINT_STRING 0x09

    %ifndef EXE
    org 0x0100
    %else
    bits 32
    section .bss class=STACK
stack:
    resb 1024                ; Allocate 1024 bytes for the stack
stack_top:
    %endif

    section .text
    global start
start:
    %ifdef EXE
    call prepare_stack
    mov ah, DOS_PRINT_STRING
    mov dx, message
    int DOS_INTERRUPT
    %endif
    call initialize
    call initialize_frequencies
    call reset_channels
    call calculate_ticks_per_beat
    call generate_sine_table

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

    %ifdef EXE
prepare_stack:
    mov ax, data
    mov ds, ax

    mov ax, stack
    mov ss, ax
    mov sp, stack_top
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

    section .data
calculate:
    db 1
dividend:
    dd 0x71AE0000
message:
    db 'Test$', 0

    section .bss
    output resd OUTPUT_CHANNELS

; Buffers
    dsp_buffer resd DSP_BUFFER_SIZE * DSPS

