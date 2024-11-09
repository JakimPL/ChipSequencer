    segment code
saw:
    movzx eax, byte [current_channel]
    lea si, [oscillator_timer + 4 * eax]
    lea di, [dividend]
    mov bx, BASE_VOLUME
    add bx, [volume]
    mov cx, BASE_VOLUME
    sub cx, [volume]
    call interpolate

.done:
    ret
