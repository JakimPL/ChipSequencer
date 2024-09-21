    section .text
step:
; Handle timing for sequencer
    cmp word [remaining_ticks], 0
    jnz .done

.load_next_note:
    movzx eax, byte [current_note]
    cmp al, byte [sequences]
    jl .next_note
    mov al, 0
    mov byte [current_note], 0

.next_note:
    call reset_envelope

; Load the note and calculate the remaining ticks
    lea si, [sequences + 1 + eax * 2]
    mov al, [si]
    mov [pitch], al
    mov al, [si + 1]
    movzx ax, al
    movzx ebx, word [ticks_per_beat]
    imul ax, bx
    mov [remaining_ticks], ax
    inc byte [current_note]

.done:
    dec word [remaining_ticks]
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

remaining_ticks:
    dw 0
current_note:
    db 0

; Sequences
sequences:
; Sequence 1
    db 6                     ; note_count
; notes
    db 0x30, 1
    db 0x32, 1
    db 0x34, 2
    db 0x37, 1
    db 0x33, 1
    db 0x32, 2
; Sequence 2
    db 6                     ; note_count
; notes
    db 0x30, 1
    db 0x32, 1
    db 0x34, 2
    db 0x37, 1
    db 0x33, 1
    db 0x32, 2
