    SEGMENT_CODE
wavetable:
.load_wavetable:
    LOAD_OFFSET ebx, oscillator_offset
    movzx ebx, byte [OSCILLATOR_WAVETABLE_WAVETABLE_INDEX + ebx]
    LOAD_VECTOR_ITEM wavetables, wavetable_offset
    mov esi, [wavetable_offset]
    inc esi
    movzx ebx, byte [ecx]
.load_timer:
    call load_timer
.load_wavetable_sample:
    call load_table_8bit_item
    shl eax, 8
    call apply_volume
    ret
