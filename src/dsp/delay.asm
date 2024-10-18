    SEGMENT_CODE
delay:
    push eax
    call load_buffer
    mov ecx, [dsp_offset]
    movzx ebx, word [DSP_DELAY_DRY + ecx]
    call multiply_by_integer

.mix:
    push eax
    mov eax, [esi]
    mov ebx, [dsp_offset]
    movzx ebx, word [DSP_DELAY_WET + ebx]
    pop dword [delay_value]
    call mix_delay
    mov edx, eax

.feedback:
    mov eax, [esi]
    mov ebx, [dsp_offset]
    movzx ebx, word [DSP_DELAY_FEEDBACK + ebx]
    pop dword [delay_value]
    call mix_delay
    mov [esi], eax

.restore_output:
    mov eax, edx

.increment_timer:
    movzx ecx, byte [current_dsp]
    mov ebx, [dsp_timer + 4 * ecx]
    inc ebx
    cmp ebx, DSP_BUFFER_SIZE
    jne .done
    sub ebx, DSP_BUFFER_SIZE
.done:
    mov [dsp_timer + 4 * ecx], ebx
    ret

load_buffer:
    movzx ecx, byte [current_dsp]
    mov ebx, [dsp_timer + 4 * ecx]
    shl ebx, 2
    imul ecx, DSP_BUFFER_SIZE
    add ebx, ecx
    lea esi, [dsp_buffer + ebx]
    ret

mix_delay:
    call multiply_by_integer
    mov ebx, [delay_value]
    call add_floats
    ret

    SEGMENT_BSS
    delay_value resd 1
