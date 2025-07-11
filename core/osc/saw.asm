    SEGMENT_CODE
saw:
    %ifdef USED_OSCILLATOR_SAW
    LOAD_OFFSET ebx, oscillator_offset
    mov dl, [OSCILLATOR_SAW_REVERSE + ebx]
    movzx eax, byte [current_channel]
    lea esi, [CDECL(oscillator_timer) + 4 * eax]
    mov edi, dividend
    mov bx, BASE_VOLUME
    add bx, [volume]
    mov cx, BASE_VOLUME
    sub cx, [volume]
    cmp dl, 0
    je .interpolate
.swap:
    xchg bx, cx
.interpolate:
    xor dl, dl
    call interpolate
    movzx eax, ax

.done:
    %endif
    ret
