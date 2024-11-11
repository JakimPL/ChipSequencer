    SEGMENT_CODE
saw:
    movzx eax, byte [current_channel]
    lea esi, [oscillator_timer + 4 * eax]
    lea edi, [dividend]
    mov bx, BASE_VOLUME
    add bx, [volume]
    mov cx, BASE_VOLUME
    sub cx, [volume]
    call interpolate

.done:
    ret
