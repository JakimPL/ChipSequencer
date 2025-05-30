    SEGMENT_CODE
square:
    %ifdef USED_OSCILLATOR_SQUARE
    call load_timer
.duty_cycle:
    LOAD_OFFSET ebx, oscillator_offset
    movzx ebx, word [OSCILLATOR_SQUARE_DUTY_CYCLE + ebx]
    shr ecx, 16
    imul ecx, ebx
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
    %endif
    ret
