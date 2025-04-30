    SEGMENT_CODE
step:
    LOAD_OFFSET ecx, channel_offset
    cmp byte [CHANNEL_ORDER_INDEX + ecx], -1
    jz .done

    movzx ecx, byte [current_channel]
    cmp dword [sequence_timer + 4 * ecx], 0
    jnz .done

.load_next_note:
    movzx ebx, byte [current_channel]
    movzx eax, byte [sequence_current_note + ebx]
    LOAD_OFFSET ecx, sequence_offset
; divided by 2 because each note is 2 bytes
    mov cl, byte [ecx]
    shr cl, 1
    cmp al, cl
    jl .next_note
    mov al, 0
    mov byte [sequence_current_note + ebx], 0
    push ax
    call next_sequence
    pop ax

.next_note:
    LOAD_OFFSET ecx, sequence_offset
    lea esi, [ecx + SEQUENCE_NOTES + eax * 2]
    mov al, [esi]
.check_note_rest:
    cmp al, NOTE_REST
    jz .progress_sequence
.check_note_off:
    cmp al, NOTE_OFF
    jnz .note_on
.note_off:
    call set_release
    jmp .progress_sequence
.note_on:
    movzx ecx, byte [current_channel]
    mov [pitch + ecx], al
    call reset_envelope
.progress_sequence:
    mov al, [esi + 1]
    movzx ax, al
    movzx ebx, word [ticks_per_beat]
    imul eax, ebx
    movzx ecx, byte [current_channel]
    mov [sequence_timer + 4 * ecx], eax
    inc byte [sequence_current_note + ecx]

.done:
    movzx ecx, byte [current_channel]
    dec dword [sequence_timer + 4 * ecx]
    ret

initialize_sample_rate:
    mov eax, [sample_rate]
    shl eax, 14
    mov [dividend], eax      ; dividend is shifted by 2 to allow higher ranges of sampling rates
    ret

calculate_ticks_per_beat:
    fild dword [sample_rate]
    fmul dword [unit]
    fild word [bpm]
    fdivp st1, st0
    fistp word [ticks_per_beat]
    ret

    SEGMENT_BSS
    ticks_per_beat resw 1
