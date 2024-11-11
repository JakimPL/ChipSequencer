    SEGMENT_CODE
load_offsets:
.load_channel:
    movzx ebx, byte [current_channel]
    imul ebx, CHANNEL_SIZE
    lea ecx, [channels + ebx]
    mov [channel_offset], ecx
    mov bl, [CHANNEL_ENVELOPE_INDEX + ecx]
.load_envelope:
    imul ebx, ENVELOPE_SIZE
    lea ecx, [envelopes + ebx]
    mov [envelope_offset], ecx
.load_order:
    call check_fixed_frequency
    je .load_oscillator
    lea ecx, [orders]
    call load_item
    mov [order_offset], ecx
.load_sequence:
    LOAD_OFFSET ebx, order_offset
    movzx ecx, byte [current_channel]
    movzx ecx, byte [current_order + ecx]
    add ebx, ecx
    mov bl, [ORDER_SEQUENCES + ebx]
    lea ecx, [sequences]
    call load_item
    mov [sequence_offset], ecx
.load_oscillator:
    lea ecx, [oscillators]
    LOAD_OFFSET ebx, channel_offset
    mov bl, [CHANNEL_OSCILLATOR_INDEX + ebx]
    call load_item
    mov [oscillator_offset], ecx
    ret

reset_channels:
    mov bx, CHANNELS
    mov esi, reset_channel
    call reset
    ret

check_fixed_frequency:
    LOAD_OFFSET ebx, channel_offset
    mov bl, [CHANNEL_ORDER_INDEX + ebx]
    cmp bl, -1
    jmp check_fixed_frequency
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
    LOAD_OFFSET ecx, channel_offset
    mov edi, [CHANNEL_OUTPUT + ecx]
    mov cl, [CHANNEL_SHIFT + ecx]
    ret

    SEGMENT_BSS
    current_channel resb 1
    %ifdef ELF
    channel_offset resd 1
    envelope_offset resd 1
    order_offset resd 1
    sequence_offset resd 1
    oscillator_offset resd 1
    %else
    channel_offset resw 1
    envelope_offset resw 1
    order_offset resw 1
    sequence_offset resw 1
    oscillator_offset resw 1
    %endif
