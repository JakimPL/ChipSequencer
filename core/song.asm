    %ifndef BIN
    global bpm
    global normalizer

    extern num_channels
    extern num_dsps

    global envelopes
    global sequences
    global orders
    global oscillators
    global wavetables
    global dsps
    global channels
    global buffer_offsets

    extern dsp_input
    extern output
    %endif

    %include "core/common.asm"
    %include "core/song/data.asm"
