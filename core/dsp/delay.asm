    %ifdef USED_DSP_DELAY
    SEGMENT_CODE
delay:
    call load_dsp_buffer
    LOAD_OFFSET ecx, dsp_offset
    call load_eax_to_fpu
    push eax

    MOV_FROM_SI eax
    push eax
    movzx edx, byte [DSP_DELAY_FEEDBACK + ecx]
    call load_eax_to_fpu_and_multiply_by_word_integer
    fadd st0, st1
    call save_eax_from_fpu
    MOV_TO_SI eax

    movzx edx, byte [DSP_DELAY_WET + ecx]
    pop eax
    call load_eax_to_fpu_and_multiply_by_word_integer

    movzx edx, byte [DSP_DELAY_DRY + ecx]
    pop eax
    call load_eax_to_fpu_and_multiply_by_word_integer
    fadd st0, st1

.increment_dsp_timer:
    movzx ebx, word [DSP_DELAY_TIME + ecx]
    call calculate_number_of_samples
    call increment_dsp_timer

.done:
    call save_eax_from_fpu
    ret
    %endif
