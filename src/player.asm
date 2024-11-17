    %include "src/common.asm"

    global initialize
    global calculate
    global mix
    global output
    global reset

    global sample_rate
    global output
    global dsp_input

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
    extern wavetable_offsets

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
    call generate_wavetables

    %if PRECALCULATE
.precalculate:
    call precalculate
    %endif

    call sound_driver_initialize
    popa
    ret

    %include "src/utils.asm"
    %include "src/elements.asm"
    %include "src/vars.asm"

    SEGMENT_DATA
calculate:
    db 1
dividend:
    dd SAMPLE_RATE << 16

    SEGMENT_BSS
    output resd OUTPUT_CHANNELS
    dsp_buffer resd DSP_BUFFER_SIZE

    %ifndef ELF
    group dgroup bss data
    %endif
