    section .text

mix:
    xor eax, eax
    mov [sound], eax
    mov cl, CHANNELS
.mix_loop:
    cmp cl, 0
    je .cast_to_8_bit

    dec cl
    mov [current_channel], cl

    call load_offsets
    call step
    call increment_timer
    call play_channel

    movzx eax, ax
    add eax, [sound]
    mov [sound], eax

    mov cl, [current_channel]
    jmp .mix_loop

.cast_to_8_bit:
    mov eax, [sound]
    mov ecx, CHANNELS
    cdq
    div ecx
    mov al, ah
    mov [sound], al
    ret

.done:
    ret
