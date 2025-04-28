    SEGMENT_CODE
gainer:
    %ifdef USED_DSP_GAINER
    LOAD_OFFSET ebx, dsp_offset
    movzx ebx, word [DSP_GAINER_VOLUME + ebx]
    call multiply_by_word_integer_to_eax
    %endif
    ret
