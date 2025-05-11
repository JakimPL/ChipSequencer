    SEGMENT_CODE
commands:
    pusha
    xor ebx, ebx
.commands_loop:
    cmp bl, [num_commands_channels]
    je .done

    mov [current_commands_channel], bl
.load_commands_channel:
    LOAD_ARRAY_ITEM commands_channels, commands_channel_offset, SIZE_COMMANDS_CHANNEL
.check_commands_channel_bypass:
    movzx edx, byte [COMMANDS_CHANNEL_FLAG + ecx]
    test dl, FLAG_BYPASS
    jnz .commands_channel_done
.load_order:
    movzx ebx, byte [COMMANDS_CHANNEL_ORDER_INDEX + ecx]
    LOAD_VECTOR_ITEM orders, commands_order_offset
.load_sequence:
    call load_commands_sequence

.check_timer:
    movzx ecx, byte [current_commands_channel]
    cmp dword [commands_sequence_timer + 4 * ecx], 0
    jne .commands_channel_done

.load_command:
    LOAD_OFFSET edi, commands_sequence_offset
    mov esi, edi
.check_if_next_sequence:
    movzx ecx, byte [current_commands_channel]
    mov cl, [commands_sequence_current_command + ecx]
    cmp cl, [COMMANDS_SEQUENCE_LENGTH + edi]
    jne .load_command_offset
.load_next_sequence:
    LOAD_OFFSET ebx, commands_order_offset
    movzx ecx, byte [current_commands_channel]
    inc byte [current_commands_sequence + ecx]
    mov byte [commands_sequence_current_command + ecx], 0
    mov word [commands_sequence_current_offset + ecx], 0
    mov dl, [current_commands_sequence + ecx]
    cmp dl, [ORDER_LENGTH + ebx]
    jne .load_commands_sequence
    mov byte [current_commands_sequence + ecx], 0
.load_commands_sequence:
    pusha
    call load_commands_sequence
    popa

.load_command_offset:
    LOAD_OFFSET edi, commands_sequence_offset
    add edi, COMMANDS_SEQUENCE_DATA
    movzx ecx, word [commands_sequence_current_offset]
    add edi, ecx
.execute_command:
    movzx eax, byte [COMMAND_INSTRUCTION + edi]
    LOAD_FUNCTION instructions, eax

.load_next_command:
    movzx ecx, byte [current_commands_channel]
    inc byte [commands_sequence_current_command + ecx]
    add word [commands_sequence_current_offset + 2 * ecx], ax
    jne .set_timer

.set_timer:
    movzx ebx, byte [COMMAND_DURATION + edi]
    mov eax, [ticks_per_beat]
    mul ebx
    mov [commands_sequence_timer + 4 * ecx], eax

.commands_channel_done:
    movzx ebx, byte [current_commands_channel]
    dec dword [commands_sequence_timer + 4 * ebx]
    inc bl
    jmp .commands_loop
.done:
    popa
    ret

load_commands_sequence:
    LOAD_OFFSET ebx, commands_order_offset
    movzx ecx, byte [current_commands_channel]
    movzx ecx, byte [current_commands_sequence + ecx]
    add ebx, ecx
    movzx ebx, byte [ORDER_SEQUENCES + ebx]
    LOAD_VECTOR_ITEM_16_BIT commands_sequences, commands_sequence_offset
    ret

reset_commands_channels:
    movzx bx, [num_commands_channels]
    mov esi, reset_commands_channel
    call reset
    ret

reset_commands_channel:
    mov byte [current_commands_channel], bl
    movzx ecx, byte [current_commands_channel]
    mov dword [commands_sequence_timer + 4 * ecx], 0
    mov byte [current_commands_sequence + ecx], 0
    mov byte [commands_sequence_current_command + ecx], 0
    mov word [commands_sequence_current_offset + ecx], 0
    ret

; Commands
    %include "core/commands/empty.asm"
    %include "core/commands/port.asm"
    %include "core/commands/gainer.asm"
    %include "core/commands/bpm.asm"
    %include "core/commands/division.asm"
    %include "core/commands/change.asm"

    SEGMENT_DATA
instructions:
    %ifndef BITS_16
    dd empty
    dd portamento_up
    dd portamento_down
    dd set_master_gainer
    dd set_division
    dd change_byte_value
    dd change_word_value
    dd change_dword_value
    dd change_float_value
    %else
    dw empty
    dw portamento_up
    dw portamento_down
    dw set_master_gainer
    dw set_division
    dw change_byte_value
    dw change_word_value
    dw change_dword_value
    dw change_float_value
    %endif

    SEGMENT_BSS
    current_commands_channel resb 1
    %ifndef BITS_16
    commands_channel_offset resd 1
    commands_order_offset resd 1
    commands_sequence_offset resd 1
    %else
    commands_channel_offset resw 1
    commands_order_offset resw 1
    commands_sequence_offset resw 1
    %endif
