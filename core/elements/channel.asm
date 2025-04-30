    SEGMENT_CODE
load_offsets:
.load_channel:
    movzx ebx, byte [current_channel]
    LOAD_ARRAY_ITEM channels, channel_offset, SIZE_CHANNEL
.load_envelope:
    movzx ebx, byte [CHANNEL_ENVELOPE_INDEX + ecx]
    LOAD_ARRAY_ITEM envelopes, envelope_offset, SIZE_ENVELOPE
.load_order:
    call load_order_and_check_constant_pitch
    jnz .load_oscillator
    LOAD_VECTOR_ITEM orders, order_offset
.load_sequence:
    LOAD_OFFSET ebx, order_offset
    movzx ecx, byte [current_channel]
    movzx ecx, byte [current_sequence + ecx]
    add ebx, ecx
    movzx ebx, byte [ORDER_SEQUENCES + ebx]
    LOAD_VECTOR_ITEM sequences, sequence_offset
.load_oscillator:
    LOAD_OFFSET ebx, channel_offset
    movzx ebx, byte [CHANNEL_OSCILLATOR_INDEX + ebx]
    LOAD_VECTOR_ITEM oscillators, oscillator_offset
    ret

reset_channels:
    movzx bx, [num_channels]
    mov esi, reset_channel
    call reset
    ret

load_order_and_check_constant_pitch:
    LOAD_OFFSET ebx, channel_offset
    test byte [CHANNEL_FLAG + ebx], FLAG_CONSTANT_PITCH
    movzx ebx, byte [CHANNEL_ORDER_INDEX + ebx]
    ret

reset_channel:
    mov byte [current_channel], bl
    movzx ecx, byte [current_channel]
    mov dword [envelope_timer + 4 * ecx], 0
    mov dword [sequence_timer + 4 * ecx], 0
    mov dword [oscillator_timer + 4 * ecx], 0
    mov byte [pitch + ecx], 0
    mov byte [envelope_mode + ecx], 0
    mov byte [current_sequence + ecx], 0
    mov byte [sequence_current_note + ecx], 0
    ret

play_channel:
    call adsr
    call oscillator
    ret

load_channel_target:
    LOAD_OFFSET ecx, channel_offset
    mov edi, [CHANNEL_OUTPUT + ecx]
    mov edx, [CHANNEL_SPLITTER + ecx]
    mov cl, [CHANNEL_OUTPUT_FLAG + ecx]
    ret

    SEGMENT_BSS
    current_channel resb 1
    %ifndef BITS_16
    channel_offset resd 1
    envelope_offset resd 1
    order_offset resd 1
    sequence_offset resd 1
    oscillator_offset resd 1
    wavetable_offset resd 1
    %else
    channel_offset resw 1
    envelope_offset resw 1
    order_offset resw 1
    sequence_offset resw 1
    oscillator_offset resw 1
    wavetable_offset resw 1
    %endif
