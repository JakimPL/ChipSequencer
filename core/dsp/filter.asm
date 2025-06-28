    SEGMENT_CODE
filter:
    %ifdef USED_DSP_FILTER
.load_buffer:
    call load_dsp_buffer

    MOV_FROM_SI ebx
.load_frequency:
    LOAD_OFFSET ecx, dsp_offset
    mov dl, [DSP_FILTER_MODE + ecx]
    mov cx, [DSP_FILTER_FREQUENCY + ecx]
    shr cx, 1
    mov [cutoff_frequency], cx

.calculate_coefficients:
; x = cutoff_frequency ^ 2 / (65536 * sample_rate)
    fild word [cutoff_frequency]
    fmul st0, st0
    fild dword [CDECL(sample_rate)]
    fdiv
    fld dword [f_65536]
    fdiv

; -2 * pi * x
    fld dword [pi]
    fild word [i_2]
    fmulp st1, st0
    fmulp st1, st0
    fchs

; exp(...)
    fldl2e
    fmul
    call power

; a = exp(..), b = 1 - a
    fld1
    fsub st0, st1

.apply_filter:
; a * y[n] + b * y[n - 1]
    call load_eax_to_fpu
    fld st0
    fmulp st2, st0
    fxch st2

    mov [value], ebx
    fmul dword [value]
    faddp st1
    fst dword [value]
    mov eax, [value]
    MOV_TO_SI eax

.apply_mode:
    cmp dl, 0x80
    jc .done

.difference:
    fsub st1, st0
    fxch st1
    call save_eax_from_fpu

.done:
    fstp st0
    %endif
    ret

    %ifdef USED_DSP_FILTER
    SEGMENT_BSS
    cutoff_frequency resw 1
    %endif
