    %define OSC_SQUARE_PARAMETERS 1
    %define OSC_SQUARE_DUTY_CYCLE 0

    section .text
square:
    movzx eax, byte [current_channel]
    mov eax, [oscillator_timer + 4 * eax]
    mov ecx, [dividend]
.duty_cycle:
    shr ecx, 8
    imul ecx, 0x80
    mov ebx, eax
    cmp eax, ecx

    mov ax, BASE_VOLUME
    mov bx, [volume]

    jl .low_value
    jmp .high_value

.low_value:
    add ax, bx
    jmp .done

.high_value:
    sub ax, bx

.done:
    ret
