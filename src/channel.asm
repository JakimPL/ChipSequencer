    section .text
load_item:
; Arguments:
; ECX - beginning offset (sequence)
; BL  - element index
    cmp bl, 0
    jz .done
.find_data:
    movzx eax, byte [ecx]
    add ecx, eax
    inc ecx
    dec bl
    jnz .find_data
.done:
    ret

load_offsets:
.load_channel:
    movzx bx, byte [current_channel]
    imul bx, CHANNEL_SIZE
    lea ecx, [channels + bx]
    mov dword [channel_offset], ecx
    mov bl, [ecx + CHANNEL_ENVELOPE_INDEX]
.load_envelope:
    imul bx, ENVELOPE_SIZE
    lea ecx, [envelopes + bx]
    mov dword [envelope_offset], ecx
.load_order:
    lea ecx, [orders]
    mov ebx, [channel_offset]
    mov bl, [CHANNEL_ORDER_INDEX + ebx]
    call load_item
    mov dword [order_offset], ecx
.load_sequence:
    lea ecx, [sequences]
    movzx bx, byte [current_channel]
    movzx ebx, byte [current_order + bx]
    add ebx, [order_offset]
    mov bl, [ORDER_SEQUENCES + ebx]
    call load_item
    mov dword [sequence_offset], ecx
.load_oscillator:
    lea ecx, [oscillators]
    mov ebx, [channel_offset]
    mov bl, [CHANNEL_OSCILLATOR_INDEX + ebx]
    call load_item
    mov dword [oscillator_offset], ecx
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
    mov byte [envelope_mode + ecx], 0
    mov byte [current_order + ecx], 0
    mov byte [sequence_current_note + ecx], 0
    ret

play_channel:
    call adsr
    call oscillator
    ret

    section .bss
    current_channel resb 1
    channel_offset resd 1
    envelope_offset resd 1
    order_offset resd 1
    sequence_offset resd 1
    oscillator_offset resd 1
