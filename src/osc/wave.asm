    SEGMENT_CODE
wavetable:
.load_table_size:
    movzx ebx, word [oscillator_offset]
    movzx ebx, byte [OSCILLATOR_WAVETABLE_WAVETABLE_INDEX + ebx]
    mov ecx, wavetables
    call load_item
    movzx ebx, byte [ecx]
.load_timer:
    call load_timer

    mov si, wavetable_samples
    call load_table_item
    call apply_volume
    ret
