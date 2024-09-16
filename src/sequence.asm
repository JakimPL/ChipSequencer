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
    dw 480
note_count:
    db 8

sequence:
    db 0x25, 1
    db 0x26, 1
    db 0x27, 2
    db 0x28, 1
    db 0x29, 1
    db 0x3A, 2

current_note:
    db 0
remaining_ticks:
    dw 0
ticks_per_beat:
    dw 0
