    %include "core/common.asm"

    %ifndef BIN
    global CDECL(initialize)
    global CDECL(calculate)
    global CDECL(frame)
    global CDECL(output)
    global CDECL(reset)
    global CDECL(calculate_ticks_per_beat)
    global CDECL(ticks_per_beat)

    extern CDECL(bpm)
    extern CDECL(unit)
    extern CDECL(sample_rate)
    extern CDECL(normalizer)

    extern CDECL(envelopes)
    extern CDECL(sequences)
    extern CDECL(commands_sequences)
    extern CDECL(orders)
    extern CDECL(oscillators)
    extern CDECL(wavetables)
    extern CDECL(dsps)
    extern CDECL(channels)
    extern CDECL(commands_channels)
    extern CDECL(targets)
    %endif

    SEGMENT_CODE
CDECL(initialize):
    pusha
    call initialize_frequencies
    call initialize_sample_rate
    call CDECL(calculate_ticks_per_beat)
    call reset_channels

    %ifdef TRACKER
    call reset_global_row
    %endif

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

    popa
    ret

CDECL(frame):
    %ifdef USED_COMMAND
    call commands
    %endif
    call mix
    ret

    %include "core/utils.asm"
    %include "core/elements.asm"
    %include "core/vars.asm"

    SEGMENT_DATA
CDECL(calculate):
    db 1

    SEGMENT_BSS
    CDECL(output) resd MAX_OUTPUT_CHANNELS
    %ifdef TRACKER
    CDECL(dsp_buffer) resd MAX_DSPS * MAX_DSP_BUFFER_SIZE
    %else
    CDECL(dsp_buffer) resd DSPS * MAX_DSP_BUFFER_SIZE
    %endif
    dividend resd 1
