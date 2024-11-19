    global bpm
    global normalizer
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
