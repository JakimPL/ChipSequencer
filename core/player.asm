    %include "core/common.asm"

    %ifndef BIN
    global initialize
    global calculate
    global frame
    global output
    global reset
    global calculate_ticks_per_beat
    global ticks_per_beat

    extern sound_driver_step
    extern sound_driver_initialize
    extern sound_driver_terminate

    extern bpm
    extern unit
    extern sample_rate
    extern normalizer

    extern envelopes
    extern sequences
    extern commands_sequences
    extern orders
    extern oscillators
    extern wavetables
    extern dsps
    extern channels
    extern commands_channels
    extern buffer_offsets
    extern targets
    %endif

    SEGMENT_CODE
initialize:
    pusha
    call initialize_frequencies
    call initialize_sample_rate
    call calculate_ticks_per_beat
    call reset_channels

    %ifdef USED_COMMAND
    call reset_commands_channels
    %endif

    %ifdef USED_DSP
    call initialize_dsp_buffers
    call reset_dsps
    %endif

    %ifdef USED_OSCILLATOR_SINE
    call generate_sine_table
    %endif

    %ifdef USED_OSCILLATOR_NOISE
    call initialize_seeds
    %endif

    call sound_driver_initialize
    popa
    ret

frame:
    %ifdef USED_COMMAND
    call commands
    %endif
    call mix
    ret

    %include "core/utils.asm"
    %include "core/elements.asm"
    %include "core/vars.asm"

    SEGMENT_DATA
calculate:
    db 1

    SEGMENT_BSS
    output resd MAX_OUTPUT_CHANNELS
    %ifdef TRACKER
    dsp_buffer resd MAX_DSPS * MAX_DSP_BUFFER_SIZE
    %else
    dsp_buffer resd DSPS * MAX_DSP_BUFFER_SIZE
    %endif
    dividend resd 1
