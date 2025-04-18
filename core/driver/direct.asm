    %define PORT 0x022C
    %define DIVISOR 0x1234DE / SAMPLE_RATE

    SEGMENT_CODE
sound_driver_initialize:
.set_timer:
    mov al, 0x36             ; Command byte: binary, mode 2 (rate generator), LSB/MSB
    out 0x43, al             ; Send to the PIT command register

    mov ax, DIVISOR          ; 29102 Hz
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
    ret

sound_driver_terminate:
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
    ret

play_sound:
    mov dx, 0x022C
    mov al, 0x10
    out dx, al

; Play sound
    mov ax, [output]
    mov al, ah               ; Cast to 8-bit
    mov dx, PORT
    out dx, al

; Send End of Interrupt (EOI) signal to PIC (Programmable Interrupt Controller)
    mov al, 0x20
    out 0x20, al
    ret

sound_driver_step:
    %if PRECALCULATE
    call load_precalculated
    cmp dword [song_position], SONG_LENGTH
    jne .done
    mov dword [song_position], 0
    %else
    call mix
    %endif
.done:
    mov byte [calculate], 0
    ret

isr:
    pusha
    call play_sound
    mov byte [calculate], 1
    popa
    iret
