    SEGMENT_CODE
    %if PRECALCULATE
precalculate:
    cli
    mov dword [song_position], 0
    PRINT_STRING message_precalculating

    mov ebx, [mem_pointer]
.precalculate:
    push ebx
    call frame
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
    song_position resd 1
    mem_pointer resw 1
    %endif
