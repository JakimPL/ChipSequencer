    section .text

mix:
    xor eax, eax
    mov [sound], eax
.mix:
    mov cl, CHANNELS
.mix_loop:
    cmp cl, 0
    je .dsp

    dec cl
    mov [current_channel], cl

.process_channel:
    call load_offsets
    call step
    call increment_timer
    call play_channel
    call load_target
    call store_output
    jmp .mix_loop

.dsp:
    mov cl, EFFECTS
.dsp_loop:
    cmp cl, 0
    je .normalize

    dec cl
    mov [current_effect], cl
.process_dsp:
    jmp .dsp_loop

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

store_output:
.store_output:
    movzx eax, ax
    mov edx, [di]

.shift:
    mov bl, cl
    and bl, 0b00000111
    shr eax, cl

.check_mode:
    test cl, 0b01000000
    je .set_size
.set_mode:
    xor edx, edx
.set_size:
    test cl, 0b00010000
    jne .add_16_bit
    test cl, 0b00100000
    jne .add_8_bit
.add_32_bit:
    mov [di], edx
    add [di], eax
    jmp .restore_index
.add_16_bit:
    mov [di], dx
    add [di], ax
    jmp .restore_index
.add_8_bit:
    mov [di], dl
    add [di], al
.restore_index:
    mov cl, [current_channel]
    ret
