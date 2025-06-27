    SEGMENT_CODE
load_channel:
.load_channel:
    movzx ebx, byte [current_channel]
    LOAD_ARRAY_ITEM CDECL(channels), channel_offset, SIZE_CHANNEL
.load_envelope:
    movzx ebx, byte [CHANNEL_ENVELOPE_INDEX + ecx]
    LOAD_ARRAY_ITEM CDECL(envelopes), envelope_offset, SIZE_ENVELOPE
.load_order:
    call load_order_and_check_constant_pitch
    LOAD_VECTOR_ITEM CDECL(orders), order_offset
.load_sequence:
    LOAD_OFFSET ebx, order_offset
    movzx ecx, byte [current_channel]
    movzx ecx, byte [CDECL(current_sequence) + ecx]
    add ebx, ecx
    movzx ebx, byte [ORDER_SEQUENCES + ebx]
    LOAD_VECTOR_ITEM CDECL(sequences), sequence_offset
.load_oscillator:
    LOAD_OFFSET ebx, channel_offset
    movzx ebx, byte [CHANNEL_OSCILLATOR_INDEX + ebx]
    LOAD_VECTOR_ITEM CDECL(oscillators), oscillator_offset
    ret

load_order_and_check_constant_pitch:
    LOAD_OFFSET ebx, channel_offset
    test byte [CHANNEL_FLAG + ebx], FLAG_CONSTANT_PITCH
    movzx ebx, byte [CHANNEL_ORDER_INDEX + ebx]
    ret

reset_channels:
    movzx bx, [CDECL(num_channels)]
    mov esi, reset_channel
    call CDECL(reset)
    ret

reset_channel:
    mov byte [current_channel], bl
    movzx ecx, byte [current_channel]
    xor eax, eax
    mov [CDECL(envelope_timer) + 4 * ecx], eax
    mov [CDECL(sequence_timer) + 4 * ecx], eax
    mov [CDECL(sequence_timer_row) + ecx], al
    mov [CDECL(oscillator_timer) + 4 * ecx], eax
    mov [CDECL(pitch) + ecx], al
    mov [CDECL(envelope_mode) + ecx], al
    mov [CDECL(current_sequence) + ecx], al
    mov [CDECL(sequence_current_note) + ecx], al
    ret

play_channel:
    call adsr
    call oscillator
    ret

load_channel_target:
    LOAD_OFFSET edi, channel_offset
    mov edx, [CHANNEL_SPLITTER + edi]
    mov cl, [CHANNEL_OUTPUT_FLAG + edi]
    mov ch, [CHANNEL_FLAG + edi]
    movzx edi, byte [CHANNEL_TARGET + edi]
    mov edi, [CDECL(targets) + 4 * edi]
    ret

    SEGMENT_BSS
    current_channel resb 1
    channel_offset resd 1
    envelope_offset resd 1
    order_offset resd 1
    sequence_offset resd 1
    oscillator_offset resd 1
    wavetable_offset resd 1
