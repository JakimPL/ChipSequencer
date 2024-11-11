    SEGMENT_CODE
load_offsets:
.load_channel:
    movzx ebx, byte [current_channel]
    imul ebx, CHANNEL_SIZE
    lea cx, [channels + ebx]
    mov [channel_offset], cx
    mov bl, [CHANNEL_ENVELOPE_INDEX + ecx]
.load_envelope:
    imul ebx, ENVELOPE_SIZE
    lea cx, [envelopes + ebx]
    mov [envelope_offset], cx
.load_order:
    call check_fixed_frequency
    je .load_oscillator
    lea cx, [orders]
    call load_item
    mov [order_offset], cx
.load_sequence:
    lea cx, [sequences]
    movzx ebx, byte [current_channel]
    movzx ebx, byte [current_order + ebx]
    add bx, [order_offset]
    mov bl, [ORDER_SEQUENCES + ebx]
    call load_item
    mov [sequence_offset], cx
.load_oscillator:
    lea cx, [oscillators]
    movzx ebx, word [channel_offset]
    mov bl, [CHANNEL_OSCILLATOR_INDEX + ebx]
    call load_item
    mov [oscillator_offset], cx
    ret

reset_channels:
    mov bx, CHANNELS
    mov esi, reset_channel
    call reset
    ret

check_fixed_frequency:
    movzx ebx, word [channel_offset]
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
    movzx ecx, word [channel_offset]
    mov edi, [CHANNEL_OUTPUT + ecx]
    mov cl, [CHANNEL_SHIFT + ecx]
    ret

    SEGMENT_BSS
    current_channel resb 1
    channel_offset resw 1
    envelope_offset resw 1
    order_offset resw 1
    sequence_offset resw 1
    oscillator_offset resw 1
