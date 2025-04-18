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

integer_to_float:
; EAX - input/output
    sub eax, BASE_VOLUME

    mov dword [value], eax
    fild dword [value]
    fld dword [half_range]
    fdiv
    fstp dword [value]
    mov eax, [value]
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

multiply_by_integer:
; EAX - input float, output
; EBX - input 16-bit integer
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

add_floats:
; EAX - input/output
; EBX - input float
    mov dword [value], ebx
    fld dword [value]
    mov dword [value], eax
    fld dword [value]
    fadd
    fstp dword [value]
    fstp st0
    mov eax, dword [value]
    ret

    SEGMENT_DATA
pi:
    dd __float32__(3.141592653589793238462)
angle_constant:
    dd ANGLE_CONSTANT
half_range:
    dd __float32__(32767.5)
two:
    dw 2
    %ifndef ELF
sample_rate:
    dd SAMPLE_RATE
    %endif
y:
    dd 0

    SEGMENT_BSS
    angle resd 1
    value resd 1
    sine_table resw TABLE_SIZE
