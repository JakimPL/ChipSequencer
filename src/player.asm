    bits 16
    org 100h

    section .text
start:
; Set up the PIT to generate an interrupt at the desired sampling rate
    mov al, 0x36             ; Command byte: binary, mode 2 (rate generator), LSB/MSB
    out 0x43, al             ; Send to the PIT command register

    mov al, [divisor]        ; 29102 Hz
    out 0x40, al             ; Set low byte (LSB)
    mov al, ah
    out 0x40, al             ; Set high byte (MSB)

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

.main_loop:
    jmp .main_loop
    cmp word [index], 0xFFFE

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

    call subroutine
    call increment_timer

; Send End of Interrupt (EOI) signal to PIC (Programmable Interrupt Controller)
    mov al, 0x20
    out 0x20, al

    pop dx
    pop cx
    pop bx
    pop ax

    iret

    %include "SRC\SOUND.ASM"

    section .data
port:
    dw 0x022C
divisor:
    db 0x29
frequency:
    dd 0x01B80000
dividend:
    dd 0x71AE0000
index:
    dd 0
volume:
    dw 0x7FFF
