    SEGMENT_CODE
filter:
.load_buffer:
    movzx ecx, byte [current_dsp]
    mov esi, [buffer_offsets + 2 * ecx]
    add esi, dsp_buffer

    MOV_FROM_SI ebx
.load_frequency:
    LOAD_OFFSET ecx, dsp_offset
    mov cx, [DSP_FILTER_FREQUENCY + ecx]
    mov [cutoff_frequency], cx

.calculate_coefficients:
    fild word [cutoff_frequency]
    fild word [sample_rate]
    fdiv

; -2 * pi * cutoff_frequency / sample_rate
    fld dword [pi]
    fild word [two]
    fmulp st1, st0
    fmulp st1, st0
    fchs

; exp(...)
    fldl2e
    fmul
    fld st0
    frndint
    fsub st1, st0
    fxch
    f2xm1
    fld1
    fadd
    fscale
    fstp st1

; a = exp(..), b = 1 - a
    fld1
    fsub st0, st1

.apply_filter:
; a * y[n] + b * y[n - 1]
    mov [value], eax
    fld dword [value]
    fmulp st1, st0

    mov [value], ebx
    fld dword [value]
    fmulp st2, st0

    faddp st1, st0
    fstp dword [value]
    mov eax, [value]
    MOV_TO_SI eax
    ret

    SEGMENT_BSS
    cutoff_frequency resw 1
