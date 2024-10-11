    section .text
gainer:
    mov ebx, [dsp_offset]
    movzx ebx, word [DSP_GAINER_VOLUME + ebx]
    mov eax, __float32__(0.0)
    ret
