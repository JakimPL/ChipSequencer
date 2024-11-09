    segment code
sine:
    call load_timer
    mov ebx, TABLE_SIZE
    mov si, sine_table
    call load_table_item
    call apply_volume
    ret
