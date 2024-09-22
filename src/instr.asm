    section .text
load_offsets:
; Load the instrument
    movzx bx, byte [current_instrument]
    imul bx, INSTRUMENT_SIZE
    lea ecx, [instruments + bx]
    mov dword [instrument_offset], ecx
    mov bl, [instruments + INSTRUMENT_ENVELOPE_INDEX + bx]
; Load the envelope
    imul bx, ENVELOPE_SIZE
    lea ecx, [envelopes + bx]
    mov dword [envelope_offset], ecx
; Load the sequence
    lea ecx, [sequences]
    movzx bx, byte [current_instrument]
    mov bl, [instruments + INSTRUMENT_SEQUENCE_INDEX + bx]
    cmp bl, 0
    jz .done
.find_sequence:
    movzx eax, byte [ecx]
    imul eax, 2
    add ecx, eax
    inc ecx
    dec bl
    jnz .find_sequence
.done:
    mov dword [sequence_offset], ecx
    ret

play_sample:
    call adsr
    call oscillator
.cast_to_8_bit:
    mov al, ah
    mov [sound], al
    ret

current_instrument:
    db 0
instrument_count:
    db 1
instrument_offset:
    dd 0
envelope_offset:
    dd 0
sequence_offset:
    dd 0
oscillator_offset:
    dd 0

; Instruments table
instruments:
    db 1                     ; envelope_index
    db 0                     ; envelope_mode
    dd 0                     ; envelope_timer
    db 1                     ; sequence_index
    dd 0                     ; sequence_remaining_ticks
    db 0                     ; sequence_current_note
    db 0                     ; oscillator_index
    dd 0                     ; oscillator_timer
    db 0                     ; oscillator_pitch
