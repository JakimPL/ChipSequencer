    section .text

mix:
    xor eax, eax
    mov [sound], eax
    mov cl, CHANNELS
.mix_loop:
    cmp cl, 0
    je .normalize

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

.normalize:
    mov eax, [sound]
    mov ecx, CHANNELS
    cdq
    div ecx
    mov [sound], ax
    ret

.done:
    ret
