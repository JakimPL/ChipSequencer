    section .text
saw:
    movzx eax, byte [current_channel]
    lea si, [oscillator_timer + 4 * eax]
    lea di, [dividend]
    mov bx, BASE_VOLUME
    add bx, word [volume]
    mov cx, BASE_VOLUME
    sub cx, word [volume]
    call interpolate

.done:
    ret
