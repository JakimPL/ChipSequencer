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
    mov dx, 0x01E4
    int 0x31
    jc return_to_dos

    mov [mem_handler], ax
    mov [mem_pointer], ebx

    mov word [song_position], 0
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

    mov dword [ebx], eax
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
    mov eax, [eax]
    mov [output], eax
    inc dword [song_position]
    ret

    SEGMENT_DATA
message_precalculating:
    db "Precalculating...", 13, 10, '$'
message_done:
    db "Playing!", 13, 10, '$'

    SEGMENT_BSS
mem_handler:
    dw 0
mem_pointer:
    dd 0
song_position:
    dw 0
