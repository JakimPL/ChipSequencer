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

    %include "src/common.asm"
    %include "src/song/data.asm"
