    SEGMENT_CODE
delay:
    mov [delay_value], eax
    call load_buffer
    mov ecx, [dsp_offset]
    movzx ebx, word [DSP_DELAY_DRY + ecx]
    call multiply_by_integer

.mix:
    mov [delay_dry_value], eax
    mov eax, [esi]
    mov ebx, [dsp_offset]
    movzx ebx, word [DSP_DELAY_WET + ebx]
    call multiply_by_integer
    mov ebx, [delay_dry_value]
    call add_floats
    mov edx, eax

.feedback:
    mov ebx, [dsp_offset]
    movzx ebx, word [DSP_DELAY_FEEDBACK + ebx]
    call multiply_by_integer
    mov ebx, [delay_value]
    call add_floats
    mov [esi], eax

.restore_output:
    mov eax, edx

.increment_timer:
    mov ecx, byte [dsp_offset]
    movzx edx, word [DSP_DELAY_TIME + ecx]
    movzx ecx, byte [current_dsp]
    mov ebx, [dsp_timer + 4 * ecx]
    inc ebx
    cmp ebx, DSP_BUFFER_SIZE
    jne .done
    sub ebx, DSP_BUFFER_SIZE
    mov [dsp_timer + 4 * ecx], ebx
.done:
    ret

load_buffer:
    movzx ecx, byte [current_dsp]
    mov ebx, [dsp_timer + 4 * ecx]
    shl ebx, 2
    imul ecx, DSP_BUFFER_SIZE
    add ebx, ecx
    lea esi, [dsp_buffer + ebx]
    ret

    SEGMENT_BSS
    delay_value resd 1
    delay_dry_value resd 1
