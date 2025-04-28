    SEGMENT_CODE
sine:
    %ifdef USED_OSCILLATOR_SINE
    call load_timer
    mov ebx, TABLE_SIZE
    mov esi, sine_table
    call load_table_16bit_item
    call apply_volume
    %endif
    ret
