    section .text
load_offsets:
; Load the channel
    movzx bx, byte [current_channel]
    imul bx, CHANNEL_SIZE
    lea ecx, [channels + bx]
    mov dword [channel_offset], ecx
    mov bl, [ecx + CHANNEL_ENVELOPE_INDEX]
; Load the envelope
    imul bx, ENVELOPE_SIZE
    lea ecx, [envelopes + bx]
    mov dword [envelope_offset], ecx
; Load the sequence
    lea ecx, [sequences]
    mov ebx, [channel_offset]
    mov bl, [ebx + CHANNEL_SEQUENCE_INDEX]
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

reset_channels:
    mov bl, CHANNELS
.reset_loop:
    cmp bl, 0
    jz .done
    mov byte [current_channel], bl
    call reset_channel
    dec bl
    jmp .reset_loop
.done:
    ret

reset_channel:
    movzx ecx, byte [current_channel]
    mov dword [envelope_timer + 4 * ecx], 0
    mov dword [sequence_timer + 4 * ecx], 0
    mov dword [oscillator_timer + 4 * ecx], 0
    mov byte [pitch + ecx], 0
    ret

play_channel:
    call adsr
    call oscillator
    ret

    section .bss
    current_channel resb 1
    channel_offset resd 1
    envelope_offset resd 1
    sequence_offset resd 1
    oscillator_offset resd 1
