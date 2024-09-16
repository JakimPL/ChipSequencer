    section .text
step:
; Handle timing for sequencer
    cmp word [remaining_ticks], 0
    jnz .done

.load_next_note:
    movzx eax, byte [current_note]
    cmp al, byte [note_count]
    jl .next_note
    mov al, 0
    mov byte [current_note], 0

.next_note:
; Reset ADSR timers
    mov word [global_timer], 0
    mov byte [mode], 0

; Load the note and calculate the remaining ticks
    lea si, [sequence + eax * 2]
    mov al, [si]
    mov [pitch], al
    mov al, [si+1]
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
note_count:
    db 6

sequence:
    db 0x30, 1
    db 0x32, 1
    db 0x34, 2
    db 0x37, 1
    db 0x33, 1
    db 0x32, 2

current_note:
    db 0
remaining_ticks:
    dw 0
ticks_per_beat:
    dw 0
