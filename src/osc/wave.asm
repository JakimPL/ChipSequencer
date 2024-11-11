    SEGMENT_CODE
wavetable:
.load_wavetable:
    LOAD_OFFSET ebx, oscillator_offset
    movzx ebx, byte [OSCILLATOR_WAVETABLE_WAVETABLE_INDEX + ebx]
.load_wavetable_offset:
    movzx ecx, word [wavetable_offsets + 2 * ebx]
    lea esi, [wavetable_samples + 2 * ecx]
.load_wavetable_size:
    mov ecx, wavetables
    call load_item
    movzx ebx, byte [ecx]
.load_timer:
    call load_timer
.load_wavetable_sample:
    call load_table_item
    call apply_volume
    ret
