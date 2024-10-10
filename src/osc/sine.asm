sine:
    movzx eax, byte [current_channel]
    mov eax, [oscillator_timer + 4 * eax]
    mov ecx, [dividend]

    mov ebx, TABLE_SIZE
    mul ebx
    div ecx
    movzx eax, ax
    mov ax, word [sine_table + 2 * eax]

.volume:
    movzx ebx, word [volume]
    imul ebx
    shr eax, 15
    sub ax, [volume]
    add ax, BASE_VOLUME

; imul eax, [volume]
; shr eax, 32
; add eax, BASE_VOLUME
.done:
    ret
