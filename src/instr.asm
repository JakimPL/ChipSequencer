    section .text
load_offsets:
; Load the instrument
    movzx bx, byte [current_instrument]
    imul bx, INSTRUMENT_SIZE
    lea ecx, [instruments + bx]
    mov dword [instrument_offset], ecx
    mov bl, [ecx + INSTRUMENT_ENVELOPE_INDEX]
; Load the envelope
    imul bx, ENVELOPE_SIZE
    lea ecx, [envelopes + bx]
    mov dword [envelope_offset], ecx
; Load the sequence
    lea ecx, [sequences]
    mov ebx, [instrument_offset]
    mov bl, [ebx + INSTRUMENT_SEQUENCE_INDEX]
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

reset_instruments:
    mov bl, INSTRUMENTS
.reset_loop:
    cmp bl, 0
    jz .done
    mov byte [current_instrument], bl
    call reset_instrument
    dec bl
    jmp .reset_loop
.done:
    ret

reset_instrument:
    movzx ecx, byte [current_instrument]
    mov dword [envelope_timer + 4 * ecx], 0
    mov dword [sequence_timer + 4 * ecx], 0
    mov dword [oscillator_timer + 4 * ecx], 0
    mov byte [pitch + ecx], 0
    ret

play_instrument:
    call adsr
    call oscillator
    ret

    section .bss
    current_instrument resb 1
    instrument_offset resd 1
    envelope_offset resd 1
    sequence_offset resd 1
    oscillator_offset resd 1
