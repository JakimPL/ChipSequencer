    %define TABLE_SIZE 1024
    %define PI 3.141592653589793
    %define ANGLE_CONSTANT 0.00015542474911317905 ; 1 / (2 * TABLE_SIZE * PI)

    section .text
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
    mov cx, TABLE_SIZE
    mov di, sine_table
    fld dword [angle_constant]
.generate:
    fld dword [angle]
    fsin
    fstp dword [edi]
    add edi, 2
    fmul dword [half_range]
    fadd dword [half_range]
    fistp word [edi]
    fadd st0, st1
    fstp dword [angle]
    loop .generate
    fstp st0
    ret

    section .data
angle_constant:
    dd __float32__(ANGLE_CONSTANT)
half_range:
    dd __float32__(32767.5)

    section .bss
    angle resd 1
    value resd 1
    sine_table resw TABLE_SIZE
