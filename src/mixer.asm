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
    call load_target
    movzx eax, ax
.set_mode:
    cmp cl, 1
    je .add_16_bit
    cmp cl, 2
    je .add_8_bit
.add_32_bit:
    add [di], eax
    jmp .restore_index
.add_16_bit:
    mov [di], ax
    jmp .restore_index
.add_8_bit:
    shr eax, 8
    mov [di], al
.restore_index:
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

load_target:
    mov ecx, [channel_offset]
    mov di, word [CHANNEL_OUTPUT + ecx]
    mov cl, [CHANNEL_SHIFT + ecx]
    ret
