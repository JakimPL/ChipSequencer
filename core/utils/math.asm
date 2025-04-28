    SEGMENT_CODE
interpolate:
; Interpolates linearly between two values: BX and CX into AX
; using the interpolation point from SI and the ratio from DI
    movzx eax, bx
    movzx ecx, cx
    sub eax, ecx
    mov ecx, [esi]

    imul ecx
    idiv dword [edi]

    neg ax
    add ax, bx
    ret

reduce:
; If index > dividend, let index := index - dividend
    cmp eax, [esi]
    jb .done
    sub eax, [esi]
    sbb ecx, 0
    stc
    ret
.done:
    clc
    ret

    %ifdef USED_OSCILLATOR_SINE
generate_sine_table:
    fninit
    mov dword [angle], __float32__(0.0)
    mov ecx, TABLE_SIZE
    mov edi, sine_table
.generate:
    dec ecx
    fld dword [angle]
    fsin
    fmul dword [half_range]
    fadd dword [half_range]
    fistp dword [value]
    mov ax, word [value]
    mov [edi], ax
    add edi, 2
    fld dword [angle]
    fadd dword [angle_constant]
    fstp dword [angle]
    cmp ecx, 0
    jne .generate
    fstp st0
    ret
    %endif

load_eax_to_fpu:
    mov [value], eax
    fld dword [value]
    ret

save_eax_from_fpu:
    fstp dword [value]
    mov eax, [value]
    ret

integer_to_float:
; EAX - input/output
    sub eax, BASE_VOLUME

    mov dword [value], eax
    fild dword [value]
    fld dword [half_range]
    fdiv
    call save_eax_from_fpu
    ret

float_to_integer:
; EAX - input/output
    mov dword [value], eax
    fld dword [value]
    fld dword [half_range]
    fmul
    fistp dword [value]

    mov eax, [value]
    add eax, BASE_VOLUME
    ret

multiply_by_byte_integer:
; FPU - input/output
; DL - input 8-bit integer
    mov dword [value], 0
    mov byte [value], dl
    fimul dword [value]
    fidiv word [i_255]
    ret

multiply_by_byte_integer_to_eax:
; EAX - input float, output
; DL - input 8-bit integer
    call load_eax_to_fpu_and_multiply_by_word_integer
    call save_eax_from_fpu
    mov eax, [value]
    ret

multiply_by_word_integer:
; FPU - input/output
; EBX - input 16-bit integer
    fmul dword [value]
    fdiv dword [half_range]
    ret

multiply_by_word_integer_to_eax:
; EAX - input float, output
; EBX - input 16-bit integer
    mov [value], ebx
    fild dword [value]
    mov [value], eax
    call multiply_by_word_integer
    call save_eax_from_fpu
    ret

load_eax_to_fpu_and_multiply_by_word_integer:
    call load_eax_to_fpu
    call multiply_by_byte_integer
    ret

    SEGMENT_DATA
pi:
    dd __float32__(3.141592653589793238462)
    %ifdef USED_OSCILLATOR_SINE
angle_constant:
    dd ANGLE_CONSTANT
    %endif
half_range:
    dd __float32__(32767.5)
two:
    dw 2
    %ifndef TRACKER
sample_rate:
    dd SAMPLE_RATE
    %endif

    SEGMENT_BSS
    angle resd 1
    value resd 1
    sine_table resw TABLE_SIZE
