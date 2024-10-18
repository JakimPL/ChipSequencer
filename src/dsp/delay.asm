    SEGMENT_CODE
delay:
    push eax
    call load_buffer
    mov ecx, [dsp_offset]
    movzx ebx, word [DSP_DELAY_DRY + ecx]
    call multiply_by_integer

    push eax
    mov eax, [esi]
    mov ebx, [dsp_offset]
    movzx ebx, word [DSP_DELAY_WET + ebx]
    call multiply_by_integer
    mov ebx, eax
    pop eax
    call add_floats
    mov edx, eax

.increment_timer:
    call load_timer
    mov [dsp_timer + 4 * ecx], ebx

.feedback:
    mov eax, [esi]
    mov ebx, [dsp_offset]
    movzx ebx, word [DSP_DELAY_FEEDBACK + ebx]
    call multiply_by_integer
    mov ebx, eax
    pop eax
    call add_floats
    mov [esi], eax

.done:
    mov eax, edx
    ret

load_buffer:
    movzx ecx, byte [current_dsp]
    mov ebx, [dsp_timer + 4 * ecx]
    shl ebx, 2
    imul ecx, DSP_BUFFER_SIZE
    add ebx, ecx
    lea esi, [dsp_buffer + ebx]
    ret

load_timer:
    movzx ecx, byte [current_dsp]
    mov ebx, [dsp_timer + 4 * ecx]
    inc ebx
    cmp ebx, DSP_BUFFER_SIZE
    jne .done
    sub ebx, DSP_BUFFER_SIZE
.done:
    ret
