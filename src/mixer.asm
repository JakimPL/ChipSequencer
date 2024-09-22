    section .text

mix:
    xor eax, eax
    mov [sound], eax
    mov cl, INSTRUMENTS
.mix_loop:
    cmp cl, 0
    je .cast_to_8_bit

    dec cl
    mov [current_instrument], cl

    call step
    call increment_timer
    call play_instrument

    movzx eax, ax
    add eax, [sound]
    mov [sound], eax

    mov cl, [current_instrument]
    jmp .mix_loop

.cast_to_8_bit:
    mov eax, [sound]
    mov ecx, INSTRUMENTS
    div ecx
    mov al, ah
    mov [sound], al
    ret

.done:
    ret
