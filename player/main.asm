    bits 32

    org $00010000
    db $7F, "ELF"
    dd 1
    dd 0
    dd $$
    dw 2
    dw 3
    dd entry
    dd entry
    dd 4

entry:
    mov al, 0x2A
    mov ebx, esp
    int 0x80
    lea edx, [ebx + 12]
    mov ebp, entry
    mov al, 2
    int 0x80
    dec eax
    js child

main_loop:
    pusha
    call mix
    popa
    jmp main_loop

child:
    call initialize

    inc eax
    mov al, 0x3F
    pop ebx
    int 0x80

    mov al, 0x0B
    lea ebx, [ebp + ((cmd_aplay + 5 - entry) & 0xFF)]
    push 0
    push cmd_44100
    push cmd_dashr
    push ebx

    mov ecx, esp
    mov bl, (cmd_aplay - $$) & 0xFF
    int 0x80

cmd_aplay:
    db "/bin/aplay", 0

cmd_dashr:
    db "-r", 0

cmd_44100:
    db "44100", 0

sound_driver_initialize:
sound_driver_terminate:
    ret

    %include "core/player.asm"
    %include "core/song/header.asm"
    %include "core/song/data.asm"
