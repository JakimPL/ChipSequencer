play_sound:
    mov dx, 22ch
    mov al, 10h
    out dx, al

; Play sound
    mov al, [sound]
    mov dx, [port]
    out dx, al

; Send End of Interrupt (EOI) signal to PIC (Programmable Interrupt Controller)
    mov al, 0x20
    out 0x20, al
    ret

    section .data
port:
    dw 0x022C
dividend:
    dd 0x71AE0000

    section .bss
    sound resb 1
