    section .text

mix:
    call clear_dsps
    xor eax, eax
    mov [output], eax
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
    call load_channel_target
    call store_output

    mov cl, [current_channel]
    jmp .mix_loop

.dsp:
    xor eax, eax
    mov cl, DSPS
.dsp_loop:
    cmp cl, 0
    je .normalize

    dec cl
    mov [current_dsp], cl
.process_dsp:
    call load_dsp
    call process_dsp
    call load_dsp_target
    call store_output

    mov cl, [current_dsp]
    jmp .dsp_loop

.normalize:
    mov eax, [output]
    mov ecx, CHANNELS + DSPS
    cdq
    div ecx
    mov [output], ax
    ret

store_output:
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
    jmp .done
.add_16_bit:
    mov [di], dx
    add [di], ax
    jmp .done
.add_8_bit:
    mov [di], dl
    add [di], al
.done:
    ret
