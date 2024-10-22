    SEGMENT_CODE
wavetable:
    call load_timer
    movzx ebx, word [oscillator_offset]
    movzx ebx, byte [wavetables]
    mov si, wavetables + 1
    call load_table_item
    call apply_volume
    ret
