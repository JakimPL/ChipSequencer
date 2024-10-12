    section .text
gainer:
    mov ebx, [dsp_offset]
    movzx ebx, word [DSP_GAINER_VOLUME + ebx]

    mov [value], ebx
    fild dword [value]
    mov [value], eax
    fld dword [value]
    fmul
    fld dword [half_range]
    fdiv
    fstp dword [value]
    mov eax, [value]
    ret
