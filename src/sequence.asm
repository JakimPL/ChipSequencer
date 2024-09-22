    section .text
step:
; Handle timing for sequencer
    call load_offsets
    mov ecx, [instrument_offset + INSTRUMENT_SEQUENCE_REMAINING_TICKS]
    cmp dword [ecx], 0
    jnz .done

.load_next_note:
    mov ebx, [instrument_offset]
    movzx eax, byte [ebx + INSTRUMENT_SEQUENCE_CURRENT_NOTE]
    mov ecx, [sequence_offset]
    cmp al, byte [ecx]
    jl .next_note
    mov al, 0
    mov byte [ebx + INSTRUMENT_SEQUENCE_CURRENT_NOTE], 0

.next_note:
    call reset_envelope

; Load the note and calculate the remaining ticks
    mov ecx, [sequence_offset]
    lea si, [ecx + SEQUENCE_NOTES + eax * 2]
    mov al, [si]
    mov [pitch], al
    mov al, [si + 1]
    movzx ax, al
    movzx ebx, word [ticks_per_beat]
    imul eax, ebx
    mov ecx, [instrument_offset + INSTRUMENT_SEQUENCE_REMAINING_TICKS]
    mov [ecx], eax

    mov ebx, [instrument_offset]
    inc byte [ebx + INSTRUMENT_SEQUENCE_CURRENT_NOTE]

.done:
    mov ecx, [instrument_offset + INSTRUMENT_SEQUENCE_REMAINING_TICKS]
    dec dword [ecx]
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
