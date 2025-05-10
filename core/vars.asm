    %ifndef BIN
    global envelope_timer
    global sequence_timer
    global oscillator_timer
    global pitch

    global envelope_mode
    global current_sequence
    global sequence_current_note

    global dsp_input
    global dsp_timer
    global output
    %endif

    SEGMENT_BSS
    envelope_timer resd MAX_CHANNELS
    sequence_timer resd MAX_CHANNELS
    commands_timer resd MAX_COMMANDS_CHANNELS
    oscillator_timer resd MAX_CHANNELS
    pitch resb MAX_CHANNELS

    envelope_mode resb MAX_CHANNELS
    current_sequence resb MAX_CHANNELS
    sequence_current_note resb MAX_CHANNELS

    dsp_input resd MAX_DSPS
    dsp_timer resd MAX_DSPS

    wavetable_samples resw MAX_WAVETABLE_SIZE
