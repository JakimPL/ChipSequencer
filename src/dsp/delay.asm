    SEGMENT_CODE
delay:
    push eax
    call load_buffer
    movzx ecx, word [dsp_offset]
    movzx ebx, word [DSP_DELAY_DRY + ecx]
    call multiply_by_integer

.mix:
    push eax
    mov eax, [si]
    movzx ebx, word [dsp_offset]
    movzx ebx, word [DSP_DELAY_WET + ebx]
    pop dword [delay_value]
    call mix_delay
    mov edx, eax

.feedback:
    mov eax, [si]
    movzx ebx, word [dsp_offset]
    movzx ebx, word [DSP_DELAY_FEEDBACK + ebx]
    pop dword [delay_value]
    call mix_delay
    mov [si], eax

.restore_output:
    mov eax, edx

.increment_timer:
    movzx edx, word [dsp_offset]
    mov dx, [DSP_DELAY_TIME + edx]
    movzx ecx, byte [current_dsp]
    mov bx, [dsp_timer + 2 * ecx]
    inc bx
    cmp bx, dx
    jne .done
    sub bx, dx
.done:
    mov [dsp_timer + 4 * ecx], bx
    ret

load_buffer:
    movzx cx, byte [current_dsp]
    movzx bx, [dsp_timer + 4 * ecx]
    shl bx, 2
    imul cx, DSP_BUFFER_SIZE
    add bx, cx
    lea si, [dsp_buffer]
    add si, bx
    ret

mix_delay:
    call multiply_by_integer
    mov ebx, [delay_value]
    call add_floats
    ret

    SEGMENT_BSS
    delay_value resd 1
