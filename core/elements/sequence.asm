    SEGMENT_CODE
step:
    LOAD_OFFSET ecx, channel_offset
    cmp byte [CHANNEL_ORDER_INDEX + ecx], CONSTANT_ORDER
    jz .done

    movzx ecx, byte [current_channel]
    cmp byte [sequence_timer_row + ecx], 0
    jnz .decrease_timer

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
    mov al, [NOTE_PITCH + esi]
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
    movzx edx, byte [current_channel]
    mov [pitch + edx], al
.load_frequency:
    shl ax, 2
    mov ebx, frequencies
    add ebx, eax
    mov ebx, [ebx]
    mov [frequency + 4 * edx], ebx
    call reset_envelope
.progress_sequence:
    movzx ecx, byte [current_channel]
    inc byte [sequence_current_note + ecx]
    mov dword [sequence_timer + 4 * ecx], 1
    mov al, [NOTE_DURATION + esi]
    inc al
    mov [sequence_timer_row + ecx], al

.decrease_timer:
    dec dword [sequence_timer + 4 * ecx]
    jnz .done

.decrease_row:
    dec byte [sequence_timer_row + ecx]
    mov ebx, [ticks_per_beat]
    mov [sequence_timer + 4 * ecx], ebx

.done:
    ret

initialize_sample_rate:
    mov eax, [sample_rate]
    shl eax, 14
    mov [dividend], eax      ; dividend is shifted by 2 to allow higher ranges of sampling rates
    ret

calculate_ticks_per_beat:
    fild dword [sample_rate]
    fmul dword [unit]
    fidiv word [bpm]
    fistp dword [ticks_per_beat]
    ret

    SEGMENT_BSS
    ticks_per_beat resd 1
