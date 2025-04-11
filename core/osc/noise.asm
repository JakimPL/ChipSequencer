    SEGMENT_CODE
noise:
    mov ax, [noise_seed]
    mov cx, ax
    shr ax, 1
    and cx, 1
    jz .no_xor
    xor ax, 0xB400
.no_xor:
    mov [noise_seed], ax
    ret

    SEGMENT_DATA
noise_seed:
    dw 0xACE1
