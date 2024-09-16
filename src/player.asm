    bits 16
    org 100h

    section .text
start:
; Set up the PIT to generate an interrupt at the desired sampling rate
.set_timer:
    mov al, 0x36             ; Command byte: binary, mode 2 (rate generator), LSB/MSB
    out 0x43, al             ; Send to the PIT command register

    mov al, [divisor]        ; 29102 Hz
    out 0x40, al             ; Set low byte (LSB)
    mov al, ah
    out 0x40, al             ; Set high byte (MSB)

.clear_interrupts:
    cli                      ; Clear interrupts while setting vector
    xor ax, ax               ; Segment for IVT (0000h)
    mov es, ax
    mov word [es:8*4], isr   ; Set ISR offset at INT 08h (IRQ0)
    mov word [es:8*4+2], cs  ; Set ISR segment
    sti                      ; Enable interrupts

; Unmask IRQ0 in the PIC
    in al, 0x21              ; Read PIC mask register
    and al, 0xFE             ; Clear mask for IRQ0 (unmask)
    out 0x21, al             ; Write back to PIC

    call calculate_ticks_per_beat

main_loop:
    cmp byte [calculate], 1
    jne main_loop

    call increment_timer
    call adsr
    call subroutine

    mov byte [calculate], 0
    jmp main_loop

.exit:
    cli
    mov al, 0x36             ; Reset PIT to a known state (optional)
    out 0x43, al
    mov al, 0                ; Set divisor to maximum, stopping the PIT effectively
    out 0x40, al
    out 0x40, al

; Mask IRQ0 to prevent further interrupts
    in al, 0x21
    or al, 0x01              ; Set mask for IRQ0 (disable)
    out 0x21, al

; Return to DOS
    mov ah, 0x4C
    int 0x21

isr:
    push ax
    push bx
    push cx
    push dx

; Handle timing for sequencer
    dec word [remaining_ticks]
    jnz .play_sound

.load_next_note:
    movzx eax, byte [current_note]
    movzx ebx, byte [note_count]
    cmp ax, bx
    jl .next_note
    mov byte [current_note], 0

.next_note:
; Reset ADSR timers
    mov word [global_timer], 0
    mov byte [mode], 0

    lea si, [sequence + eax * 2]
    mov al, [si]
    mov [pitch], al
    mov al, [si+1]
    movzx ax, al
    movzx ebx, word [ticks_per_beat]
    imul ax, bx
    mov [remaining_ticks], ax
    inc byte [current_note]

.play_sound:
    call play_sound
    mov byte [calculate], 1

    pop dx
    pop cx
    pop bx
    pop ax
    iret

    %include "SRC\SOUND.ASM"
    %include "SRC\ADSR.ASM"
    %include "SRC\SEQUENCE.ASM"

    section .data
calculate:
    db 1
divisor:
    db 0x29
