    SEGMENT_BSS
    envelope_timer resd MAX_CHANNELS
    sequence_timer resd MAX_CHANNELS
    oscillator_timer resd MAX_CHANNELS
    pitch resb MAX_CHANNELS

    envelope_mode resb MAX_CHANNELS
    current_order resb MAX_CHANNELS
    sequence_current_note resb MAX_CHANNELS

    dsp_input resd MAX_DSPS
    dsp_timer resw MAX_DSPS

    wavetable_samples resw MAX_WAVETABLE_SIZE
