    section .text
gainer:
    mov ebx, [dsp_offset]
    movzx ebx, word [DSP_GAINER_VOLUME + ebx]

; sub eax, BASE_VOLUME
; imul eax, ebx
; sar eax, 7
; add eax, BASE_VOLUME
    ret
