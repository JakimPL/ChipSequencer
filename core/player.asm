    %include "core/common.asm"

    global initialize
    global calculate
    global mix
    global output
    global reset
    global calculate_ticks_per_beat
    global ticks_per_beat

    global sample_rate
    global output
    global dsp_input

    global envelope_timer
    global sequence_timer
    global oscillator_timer
    global pitch

    global current_sequence
    global sequence_current_note

    extern sound_driver_step
    extern sound_driver_initialize
    extern sound_driver_terminate

    extern bpm
    extern normalizer
    extern envelopes
    extern sequences
    extern orders
    extern oscillators
    extern wavetables
    extern dsps
    extern channels
    extern buffer_offsets

    %ifn DIRECT_MODE
    global buffer
    SEGMENT_BSS
    align 2
    buffer resb SB_BUFFER_SIZE * (1 + SB_16BIT)
    %endif

    SEGMENT_CODE
initialize:
    pusha
    call initialize_frequencies
    call calculate_ticks_per_beat

    call initialize_dsp_buffers
    call reset_channels
    call reset_dsps

    call generate_sine_table

    %if PRECALCULATE
.precalculate:
    call precalculate
    %endif

    call sound_driver_initialize
    popa
    ret

    %include "core/utils.asm"
    %include "core/elements.asm"
    %include "core/vars.asm"

    SEGMENT_DATA
calculate:
    db 1
dividend:
    dd SAMPLE_RATE << 16

    SEGMENT_BSS
    output resd MAX_OUTPUT_CHANNELS
    %ifdef ELF
    dsp_buffer resd MAX_DSPS * MAX_DSP_BUFFER_SIZE
    %else
    dsp_buffer resd DSPS * MAX_DSP_BUFFER_SIZE
    %endif

    %ifndef ELF
    group dgroup bss data
    %endif
