    section .text
square:
    movzx eax, byte [current_channel]
    mov eax, [oscillator_timer + 4 * eax]
    mov ecx, [dividend]
    shr ecx, 1
    mov ebx, eax
    cmp eax, ecx

    jl .low_value
    jmp .high_value

.low_value:
    mov ax, BASE_VOLUME
    mov bx, [volume]
    add ax, bx
    jmp .done

.high_value:
    mov ax, BASE_VOLUME
    mov bx, [volume]
    sub ax, bx

.done:
    ret
