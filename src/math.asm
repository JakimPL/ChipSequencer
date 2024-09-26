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
