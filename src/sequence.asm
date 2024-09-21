    section .text
step:
; Handle timing for sequencer
    call load_offsets
    cmp dword [remaining_ticks], 0
    jnz .done

.load_next_note:
    movzx eax, byte [current_note]
    mov ecx, [sequence_offset]
    cmp al, byte [ecx]
    jl .next_note
    mov al, 0
    mov byte [current_note], 0

.next_note:
    call reset_envelope

; Load the note and calculate the remaining ticks
    mov ecx, [sequence_offset]
    lea si, [ecx + 1 + eax * 2]
    mov al, [si]
    mov [pitch], al
    mov al, [si + 1]
    movzx ax, al
    movzx ebx, word [ticks_per_beat]
    imul eax, ebx
    mov [remaining_ticks], eax
    inc byte [current_note]

.done:
    dec dword [remaining_ticks]
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
    dd 0
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
    db 8                     ; note_count
; notes
    db 0x30, 3
    db 0x33, 1
    db 0x30, 2
    db 0x33, 1
    db 0x30, 1
    db 0x33, 1
    db 0x36, 1
    db 0x2F, 6
