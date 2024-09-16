    section .text

mix:
    mov cx, [instrument_count]
    xor ax, ax
    mov di, sound_buffer
.mix_loop:
    cmp cx, 0
    je .done

; Mix current instrument’s sound sample
    mov al, [sound_sample]
    add [di], al

; Move to next instrument
    add si, instrument_size
    dec cx
    jmp .mix_loop

.done:
    ret
