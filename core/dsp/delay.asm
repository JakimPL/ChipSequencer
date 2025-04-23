    SEGMENT_CODE
delay:
    call load_dsp_buffer
    LOAD_OFFSET ecx, dsp_offset
    movzx edx, byte [DSP_DELAY_DRY + ecx]
    call load_eax_to_fpu
    call multiply_by_byte_integer

.mix:
    push eax
    MOV_FROM_SI eax
    movzx edx, byte [DSP_DELAY_WET + ecx]
    call load_eax_to_fpu
    call multiply_by_byte_integer
    fadd st0, st1

.increment_dsp_timer:
    movzx edx, word [DSP_DELAY_TIME + ecx]
    call increment_dsp_timer

.done:
    pop eax
    MOV_TO_SI eax
    call save_eax_from_fpu
    ret
