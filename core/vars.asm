    %ifndef BIN
    global CDECL(envelope_timer)
    global CDECL(sequence_timer)
    global CDECL(oscillator_timer)
    global CDECL(pitch)

    global CDECL(envelope_mode)
    global CDECL(current_sequence)
    global CDECL(sequence_current_note)

    global CDECL(commands_sequence_timer)
    global CDECL(current_commands_sequence)
    global CDECL(commands_sequence_offset)
    global CDECL(commands_sequence_current_command)

    global CDECL(sequence_timer_row)
    global CDECL(commands_sequence_timer_row)
    %ifdef TRACKER
    global CDECL(global_row)
    %endif

    global CDECL(dsp_input)
    global CDECL(dsp_timer)
    global CDECL(output)
    %endif

    SEGMENT_BSS
    CDECL(envelope_timer) resd MAX_CHANNELS
    CDECL(sequence_timer) resd MAX_CHANNELS
    CDECL(oscillator_timer) resd MAX_CHANNELS
    CDECL(pitch) resb MAX_CHANNELS
    CDECL(frequency) resd MAX_CHANNELS

    CDECL(envelope_mode) resb MAX_CHANNELS
    CDECL(current_sequence) resb MAX_CHANNELS
    CDECL(sequence_current_note) resb MAX_CHANNELS

    CDECL(commands_sequence_timer) resd MAX_COMMANDS_CHANNELS
    CDECL(current_commands_sequence) resb MAX_COMMANDS_CHANNELS
    CDECL(commands_sequence_current_command) resb MAX_COMMANDS_CHANNELS
    CDECL(commands_sequence_current_offset) resw MAX_COMMANDS_CHANNELS

    CDECL(sequence_timer_row) resb MAX_CHANNELS
    CDECL(commands_sequence_timer_row) resb MAX_COMMANDS_CHANNELS

    CDECL(dsp_input) resd MAX_DSPS
    CDECL(dsp_timer) resd MAX_DSPS

    wavetable_samples resw MAX_WAVETABLE_SIZE
