    SEGMENT_CODE
load_offsets:
.load_channel:
    movzx ebx, byte [current_channel]
    imul ebx, CHANNEL_SIZE
    lea ecx, [channels + ebx]
    mov dword [channel_offset], ecx
    mov bl, [CHANNEL_ENVELOPE_INDEX + ecx]
.load_envelope:
    imul ebx, ENVELOPE_SIZE
    lea ecx, [envelopes + ebx]
    mov dword [envelope_offset], ecx
.load_order:
    call check_fixed_frequency
    je .load_oscillator
    lea ecx, [orders]
    call load_item
    mov dword [order_offset], ecx
.load_sequence:
    lea ecx, [sequences]
    movzx ebx, byte [current_channel]
    movzx ebx, byte [current_order + ebx]
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
    mov si, reset_channel
    call reset
    ret

check_fixed_frequency:
    mov ebx, [channel_offset]
    mov bl, [CHANNEL_ORDER_INDEX + ebx]
    cmp bl, -1
    ret

reset_channel:
    mov byte [current_channel], bl
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
    mov di, [CHANNEL_OUTPUT + ecx]
    mov cl, [CHANNEL_SHIFT + ecx]
    ret

    SEGMENT_BSS
    current_channel resb 1
    channel_offset resd 1
    envelope_offset resd 1
    order_offset resd 1
    sequence_offset resd 1
    oscillator_offset resd 1
