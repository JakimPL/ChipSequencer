    section .text
load_offsets:
.load_channel:
    movzx bx, byte [current_channel]
    imul bx, CHANNEL_SIZE
    lea ecx, [channels + bx]
    mov dword [channel_offset], ecx
    mov bl, [CHANNEL_ENVELOPE_INDEX + ecx]
.load_envelope:
    imul bx, ENVELOPE_SIZE
    lea ecx, [envelopes + bx]
    mov dword [envelope_offset], ecx
.load_order:
    call check_fixed_frequency
    je .load_oscillator
    lea ecx, [orders]
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

check_fixed_frequency:
    mov ebx, [channel_offset]
    mov bl, [CHANNEL_ORDER_INDEX + ebx]
    cmp bl, -1
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

load_channel_target:
    mov ecx, [channel_offset]
    mov di, word [CHANNEL_OUTPUT + ecx]
    mov cl, [CHANNEL_SHIFT + ecx]
    ret

    section .bss
    current_channel resb 1
    channel_offset resd 1
    envelope_offset resd 1
    order_offset resd 1
    sequence_offset resd 1
    oscillator_offset resd 1
