    segment code
next_sequence:
    movzx ecx, byte [current_channel]
    inc byte [current_order + ecx]
    mov bl, [current_order + ecx]
    movzx ecx, word [order_offset]
    cmp bl, byte [ecx]
    jnz .done
    movzx ecx, byte [current_channel]
    mov byte [current_order + ecx], 0
.done:
    call load_offsets.load_sequence
    ret
