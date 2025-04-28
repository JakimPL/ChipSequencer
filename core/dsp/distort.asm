    SEGMENT_CODE
distortion:
    %ifdef USED_DSP_DISTORTION
    mov [value], eax
    fld1

.load_coefficient:
    LOAD_OFFSET ecx, dsp_offset
    fild dword [DSP_DISTORTION_LEVEL + ecx]
    fdiv dword [f_65536]

.calculate_coefficient:
; p = 1 / (1 - a); a from [0, 1]
    fsubp
    fld1
    fdivrp st1

.calculate_arctan:
; arctan(x * p)
    fld st0
    fmul dword [value]
    fld1
    fpatan

.calculate_normalizer:
; arctan(p)
    fxch st0, st1
    fld1
    fpatan

; arctan(x * p) / arctan(p)
    fdivp st1, st0
    call save_eax_from_fpu
    %endif
    ret
