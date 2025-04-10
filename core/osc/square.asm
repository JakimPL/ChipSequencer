    SEGMENT_CODE
square:
    call load_timer
.duty_cycle:
    LOAD_OFFSET ebx, oscillator_offset
    movzx ebx, byte [OSCILLATOR_SQUARE_DUTY_CYCLE + ebx]
    shr ecx, 8
    imul ecx, ebx
    mov ebx, eax
    cmp eax, ecx

    mov eax, BASE_VOLUME
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
