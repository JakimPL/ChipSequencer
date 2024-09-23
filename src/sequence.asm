    section .text
step:
; Handle timing for sequencer
    call load_offsets
    movzx ecx, byte [current_channel]
    cmp dword [sequence_timer + 4 * ecx], 0
    jnz .done

.load_next_note:
    mov ebx, [channel_offset]
    movzx eax, byte [ebx + CHANNEL_SEQUENCE_CURRENT_NOTE]
    mov ecx, [sequence_offset]
    cmp al, byte [ecx]
    jl .next_note
    mov al, 0
    mov byte [ebx + CHANNEL_SEQUENCE_CURRENT_NOTE], 0

.next_note:
    call reset_envelope

; Load the note and calculate the remaining ticks
    mov ecx, [sequence_offset]
    lea si, [ecx + SEQUENCE_NOTES + eax * 2]
    mov al, [si]
    movzx ecx, byte [current_channel]
    mov [pitch + ecx], al
    mov al, [si + 1]
    movzx ax, al
    movzx ebx, word [ticks_per_beat]
    imul eax, ebx

    movzx ecx, byte [current_channel]
    mov [sequence_timer + 4 * ecx], eax

    mov ebx, [channel_offset]
    inc byte [ebx + CHANNEL_SEQUENCE_CURRENT_NOTE]

.done:
    movzx ecx, byte [current_channel]
    dec dword [sequence_timer + 4 * ecx]
    ret

calculate_ticks_per_beat:
    mov eax, [dividend]
    shr eax, 16
    mov bx, 60
    imul bx
    movzx ebx, word [bpm]
    div bx
    mov [ticks_per_beat], ax
    ret

    section .bss
    ticks_per_beat resw 1
