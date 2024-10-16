    SEGMENT_CODE
prepare_stack:
    mov ax, data
    mov ds, ax
    mov ax, stack
    mov ss, ax
    mov esp, stacktop
    ret

allocate_memory:
    mov ax, 0x0501
    mov bx, SONG_LENGTH >> 4
    xor cx, cx
    int 0x31
    jc return_to_dos

    mov [mem_handler], si
    mov [mem_handler + 2], di
    mov [mem_pointer], ebx
    ret

deallocate_memory:
    mov ax, 0x0502
    mov si, [mem_handler]
    mov di, [mem_handler + 2]
    int 0x31
    jc return_to_dos
    ret

precalculate:
    cli
    mov dword [song_position], 0
    PRINT_STRING message_precalculating

    mov ebx, [mem_pointer]
.precalculate:
    push ebx
    call mix
    pop ebx

    mov word [ebx], ax
    add ebx, 2

    inc dword [song_position]
    cmp dword [song_position], SONG_LENGTH
    jne .precalculate
.done:
    mov dword [song_position], 0
    PRINT_STRING message_done
    sti
    ret

load_precalculated:
    mov eax, [song_position]
    shl eax, 1
    add eax, [mem_pointer]
    mov ax, [eax]
    mov [output], ax
    inc dword [song_position]
    ret

    SEGMENT_DATA
message_precalculating:
    db "Precalculating...", 13, 10, '$'
message_done:
    db "Playing!", 13, 10, '$'

    SEGMENT_BSS
mem_handler:
    dd 0
mem_pointer:
    dd 0
song_position:
    dd 0
