    %define DIRECT_MODE 1
    %define DIVISOR 0x29

    bits 16
    org 100h

    section .text
start:
    call initialize
    call initialize_frequencies
    call reset_channels
    call calculate_ticks_per_beat

main_loop:
.check_esc:
    mov ah, 0x01
    int 0x16
    jz .no_key

    mov ah, 0x00
    int 0x16
    cmp al, 0x1B
    je .exit

.no_key:
    cmp byte [calculate], 1
    jnz main_loop

    call mix

    mov byte [calculate], 0
    jmp main_loop

.exit:
    call terminate

; Return to DOS
    mov ah, 0x4C
    int 0x21

isr:
    pusha
    call play_sound
    mov byte [calculate], 1
    popa
    iret

    %include "SRC\CONST.ASM"
    %include "SRC\MATH.ASM"
    %include "SRC\SONG.ASM"
    %include "SRC\OSC.ASM"
    %include "SRC\ADSR.ASM"
    %include "SRC\SEQUENCE.ASM"
    %include "SRC\ORDER.ASM"
    %include "SRC\CHANNEL.ASM"
    %include "SRC\MIXER.ASM"

    %ifdef DIRECT_MODE
    %include "SRC\DRIVER\DIRECT.ASM"
    %else
    %include "SRC\DRIVER\SB16.ASM"
    %endif

    section .data
calculate:
    db 1
dividend:
    dd 0x71AE0000

    section .bss
    sound resw 1
