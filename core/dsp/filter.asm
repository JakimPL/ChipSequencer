    SEGMENT_CODE
filter:
.load_buffer:
    %ifdef ELF
    movzx esi, byte [current_dsp]
    shl esi, 8
    add esi, dsp_buffer
    %else
    movzx ecx, byte [current_dsp]
    mov esi, [buffer_offsets + 2 * ecx]
    add esi, dsp_buffer
    %endif

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
    fild dword [sample_rate]
    fdiv
    fld dword [f_65536]
    fdiv

; -2 * pi * x
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
    cmp dl, 0
    je .done

.difference:
    fsub st1, st0
    fxch st1
    call save_eax_to_fpu

.done:
    fstp st0
    ret

    SEGMENT_BSS
    cutoff_frequency resw 1
