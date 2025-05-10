    SEGMENT_CODE
commands:
    ret

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
