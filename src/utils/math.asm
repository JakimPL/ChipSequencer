    %define TABLE_SIZE 1024
    %define PI 3.141592653589793
    %define ANGLE_CONSTANT 0.006135923151542565 ; 2 * PI / TABLE_SIZE

    SEGMENT_CODE
interpolate:
; Interpolates linearly between two values: BX and CX into AX
; using the interpolation point from SI and the ratio from DI
    movzx eax, bx
    sub eax, ecx
    mov ecx, [si]

    imul ecx
    idiv dword [di]

    neg ax
    add ax, bx
    ret

generate_sine_table:
; Generates a sine table in the sine_table buffer
    fninit
    mov dword [angle], __float32__(0.0)
    mov ecx, TABLE_SIZE
    mov di, sine_table
    fld dword [angle_constant]
.generate:
    dec ecx
    fld dword [angle]
    fsin
    fmul dword [half_range]
    fadd dword [half_range]
    fistp word [di]
    add di, 2
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
angle_constant:
    dd __float32__(ANGLE_CONSTANT)
half_range:
    dd __float32__(32767.5)

    SEGMENT_BSS
    angle resd 1
    value resd 1
    sine_table resw TABLE_SIZE
