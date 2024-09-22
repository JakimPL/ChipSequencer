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

reset_instrument:
    movzx ecx, byte [current_instrument]
    mov dword [envelope_timer + 4 * ecx], 0
    mov dword [sequence_timer + 4 * ecx], 0
    mov dword [oscillator_timer + 4 * ecx], 0
    mov byte [pitch + ecx], 0
    ret

play_sample:
    call adsr
    call oscillator
.cast_to_8_bit:
    mov al, ah
    mov [sound], al
    ret

    section .bss
    current_instrument resb 1
    instrument_offset resd 1
    envelope_offset resd 1
    sequence_offset resd 1
    oscillator_offset resd 1
