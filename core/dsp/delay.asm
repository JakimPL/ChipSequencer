    SEGMENT_CODE
delay:
    call load_dsp_buffer
    LOAD_OFFSET ecx, dsp_offset
    movzx edx, byte [DSP_DELAY_DRY + ecx]
; call multiply_by_byte_integer

    movzx cx, byte [DSP_DELAY_WET + ecx]
; call multiply_by_byte_integer
    ret
