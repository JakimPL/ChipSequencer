    section .text
step:
; Handle timing for sequencer
    cmp word [sequences], 0
    jnz .done

.load_next_note:
    movzx eax, byte [sequences + 2]
    cmp al, byte [sequences + 3]
    jl .next_note
    mov al, 0
    mov byte [sequences + 2], 0

.next_note:
; Reset ADSR timers
    mov word [envelopes + 1], 0
    mov byte [envelopes], 0

; Load the note and calculate the remaining ticks
    lea si, [sequences + 4 + eax * 2]
    mov al, [si]
    mov [pitch], al
    mov al, [si + 1]
    movzx ax, al
    movzx ebx, word [ticks_per_beat]
    imul ax, bx
    mov [sequences], ax
    inc byte [sequences + 2]

.done:
    dec word [sequences]
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

    section .data
bpm:
    dw 240
ticks_per_beat:
    dw 0

; Sequences
sequences:
    dw 0                     ; remaining_ticks
    db 0                     ; current_note
    db 6                     ; note_count
; notes
    db 0x30, 1
    db 0x32, 1
    db 0x34, 2
    db 0x37, 1
    db 0x33, 1
    db 0x32, 2
