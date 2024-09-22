    %define PORT 0x022C

play_sound:
    mov dx, 22ch
    mov al, 10h
    out dx, al

; Play sound
    mov al, [sound]
    mov dx, PORT
    out dx, al

; Send End of Interrupt (EOI) signal to PIC (Programmable Interrupt Controller)
    mov al, 0x20
    out 0x20, al
    ret

    section .data

dividend:
    dd 0x71AE0000

    section .bss
    sound resb 1
