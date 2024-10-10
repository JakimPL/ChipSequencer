sine:
    movzx eax, byte [current_channel]
    mov eax, [oscillator_timer + 4 * eax]
    mov ecx, [dividend]

    mov ebx, TABLE_SIZE
    mul ebx
    div ecx
    movzx eax, ax
    mov ax, word [sine_table + 2 * eax]
.done:
    ret
