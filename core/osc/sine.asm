    %ifdef USED_OSCILLATOR_SINE
    SEGMENT_CODE
sine:
    call load_timer
    mov ebx, TABLE_SIZE
    mov esi, sine_table
    call load_table_16bit_item
    call apply_volume
    ret
    %endif
