    SEGMENT_CODE
next_sequence:
    movzx ecx, byte [current_channel]
    inc byte [current_sequence + ecx]
    mov bl, [current_sequence + ecx]
    LOAD_OFFSET ecx, order_offset
    cmp bl, byte [ecx]
    jnz .done
    movzx ecx, byte [current_channel]
    mov byte [current_sequence + ecx], 0
.done:
    call load_channel.load_sequence
    ret
