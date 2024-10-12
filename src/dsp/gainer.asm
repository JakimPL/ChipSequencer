    section .text
gainer:
    mov ebx, [dsp_offset]
    movzx ebx, word [DSP_GAINER_VOLUME + ebx]
    ret
