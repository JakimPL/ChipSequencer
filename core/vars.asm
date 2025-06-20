    %ifndef BIN
    global envelope_timer
    global sequence_timer
    global oscillator_timer
    global pitch

    global envelope_mode
    global current_sequence
    global sequence_current_note

    global commands_sequence_timer
    global current_commands_sequence
    global commands_sequence_offset
    global commands_sequence_current_command

    global sequence_timer_row
    global commands_sequence_timer_row
    %ifdef TRACKER
    global global_row
    %endif

    global dsp_input
    global dsp_timer
    global output
    %endif

    SEGMENT_BSS
    envelope_timer resd MAX_CHANNELS
    sequence_timer resd MAX_CHANNELS
    oscillator_timer resd MAX_CHANNELS
    pitch resb MAX_CHANNELS
    frequency resd MAX_CHANNELS

    envelope_mode resb MAX_CHANNELS
    current_sequence resb MAX_CHANNELS
    sequence_current_note resb MAX_CHANNELS

    commands_sequence_timer resd MAX_COMMANDS_CHANNELS
    current_commands_sequence resb MAX_COMMANDS_CHANNELS
    commands_sequence_current_command resb MAX_COMMANDS_CHANNELS
    commands_sequence_current_offset resw MAX_COMMANDS_CHANNELS

    sequence_timer_row resb MAX_CHANNELS
    commands_sequence_timer_row resb MAX_COMMANDS_CHANNELS

    dsp_input resd MAX_DSPS
    dsp_timer resd MAX_DSPS

    wavetable_samples resw MAX_WAVETABLE_SIZE
