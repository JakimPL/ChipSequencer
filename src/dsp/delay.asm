    segment code
delay:
    push eax
    call load_dsp_buffer
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
    call increment_dsp_timer

mix_delay:
    call multiply_by_integer
    mov ebx, [delay_value]
    call add_floats
    ret

    segment bss
    delay_value resd 1
