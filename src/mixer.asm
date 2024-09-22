    section .text

mix:
    xor ax, ax
    mov [sound], ax
    mov cl, INSTRUMENTS
.mix_loop:
    cmp cl, 0
    je .cast_to_8_bit

    dec cl
    mov [current_instrument], cl

    call step
    call increment_timer
    call play_instrument

    add ax, [sound]
    mov [sound], ax

    mov cl, [current_instrument]
    jmp .mix_loop

.cast_to_8_bit:
    mov ax, [sound]
    shr ax, 2
    mov al, ah
    mov [sound], al
    ret

.done:
    ret
