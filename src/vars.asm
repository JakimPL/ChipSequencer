    segment bss
    envelope_timer resd CHANNELS
    sequence_timer resd CHANNELS
    oscillator_timer resd CHANNELS
    pitch resb CHANNELS

    envelope_mode resb CHANNELS
    current_order resb CHANNELS
    sequence_current_note resb CHANNELS

    dsp_input resd DSPS
    dsp_timer resw DSPS

    wavetable_samples resw WAVETABLE_SIZE
