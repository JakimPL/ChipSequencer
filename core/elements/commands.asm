    %ifdef USED_COMMAND
    SEGMENT_CODE
commands:
    pusha
    xor ebx, ebx
.commands_loop:
    cmp bl, [CDECL(num_commands_channels)]
    je .done

    mov [current_commands_channel], bl
.load_commands_channel:
    LOAD_ARRAY_ITEM CDECL(commands_channels), commands_channel_offset, SIZE_COMMANDS_CHANNEL
.check_commands_channel_bypass:
    movzx edx, byte [COMMANDS_CHANNEL_FLAG + ecx]
    test dl, FLAG_BYPASS
    jnz .next_commands_channel
.load_order:
    movzx ebx, byte [COMMANDS_CHANNEL_ORDER_INDEX + ecx]
    LOAD_VECTOR_ITEM CDECL(orders), commands_order_offset
.load_sequence:
    call load_commands_sequence

.check_timer:
    movzx ebx, byte [current_commands_channel]
    dec dword [CDECL(commands_sequence_timer) + 4 * ebx]
    jnz .next_commands_channel

.decrease_row:
    mov eax, [CDECL(ticks_per_beat)]
    mov [CDECL(commands_sequence_timer) + 4 * ebx], eax
    dec byte [CDECL(commands_sequence_timer_row) + ebx]
    jnz .next_commands_channel

.load_command:
    LOAD_OFFSET edi, commands_sequence_offset
    mov esi, edi
.check_if_next_sequence:
    movzx ecx, byte [current_commands_channel]
    mov cl, [CDECL(commands_sequence_current_command) + ecx]
    cmp cl, [COMMANDS_SEQUENCE_LENGTH + edi]
    jne .load_command_offset
.load_next_sequence:
    LOAD_OFFSET ebx, commands_order_offset
    movzx ecx, byte [current_commands_channel]
    inc byte [CDECL(current_commands_sequence) + ecx]
    mov byte [CDECL(commands_sequence_current_command) + ecx], 0
    mov word [CDECL(commands_sequence_current_offset) + 2 * ecx], 0
    mov dl, [CDECL(current_commands_sequence) + ecx]
    cmp dl, [ORDER_LENGTH + ebx]
    jne .load_commands_sequence
    mov byte [CDECL(current_commands_sequence) + ecx], 0
.load_commands_sequence:
    pusha
    call load_commands_sequence
    popa

.load_command_offset:
    LOAD_OFFSET edi, commands_sequence_offset
    add edi, COMMANDS_SEQUENCE_DATA
    movzx ebx, byte [current_commands_channel]
    movzx ecx, word [CDECL(commands_sequence_current_offset) + 2 * ebx]
    add edi, ecx
.execute_command:
    movzx eax, byte [COMMAND_INSTRUCTION + edi]
    LOAD_FUNCTION instructions, eax

.progress_sequence:
    movzx ebx, byte [current_commands_channel]
    inc byte [CDECL(commands_sequence_current_command) + ebx]
    add word [CDECL(commands_sequence_current_offset) + 2 * ebx], ax
    mov al, [COMMAND_DURATION + edi]
    mov [CDECL(commands_sequence_timer_row) + ebx], al
    mov eax, [CDECL(ticks_per_beat)]
    mov dword [CDECL(commands_sequence_timer) + 4 * ebx], eax

.next_commands_channel:
    inc bl
    jmp .commands_loop
.done:
    popa
    ret

load_commands_sequence:
    LOAD_OFFSET ebx, commands_order_offset
    movzx ecx, byte [current_commands_channel]
    movzx ecx, byte [CDECL(current_commands_sequence) + ecx]
    add ebx, ecx
    movzx ebx, byte [ORDER_SEQUENCES + ebx]
    LOAD_VECTOR_ITEM_16_BIT CDECL(commands_sequences), commands_sequence_offset
    ret

reset_commands_channels:
    movzx bx, [CDECL(num_commands_channels)]
    mov esi, reset_commands_channel
    call CDECL(reset)
    ret

reset_commands_channel:
    mov byte [current_commands_channel], bl
    movzx ecx, byte [current_commands_channel]
    xor eax, eax
    mov [CDECL(current_commands_sequence) + ecx], al
    mov [CDECL(commands_sequence_current_command) + ecx], al
    mov [CDECL(commands_sequence_current_offset) + 2 * ecx], ax
    inc al
    mov [CDECL(commands_sequence_timer) + 4 * ecx], eax
    mov [CDECL(commands_sequence_timer_row) + ecx], al
    ret

; Commands
    %include "core/commands/empty.asm"
    %include "core/commands/port.asm"
    %include "core/commands/gainer.asm"
    %include "core/commands/tempo.asm"
    %include "core/commands/change.asm"

    SEGMENT_DATA
instructions:
    dd empty
    dd portamento_up
    dd portamento_down
    dd set_master_gainer
    dd set_bpm
    dd set_division
    dd change_float_value
    dd change_byte_value
    dd change_word_value
    dd change_dword_value
    dd add_float_value
    dd add_byte_value
    dd add_word_value
    dd add_dword_value

    SEGMENT_BSS
    current_commands_channel resb 1
    commands_channel_offset resd 1
    commands_order_offset resd 1
    commands_sequence_offset resd 1
    %endif
