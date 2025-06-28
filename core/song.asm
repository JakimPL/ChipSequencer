    %ifndef BIN
    global CDECL(bpm)
    global CDECL(normalizer)

    extern CDECL(num_channels)
    extern CDECL(num_dsps)
    extern CDECL(num_commands_channels)

    global CDECL(envelopes)
    global CDECL(sequences)
    global CDECL(orders)
    global CDECL(oscillators)
    global CDECL(wavetables)
    global CDECL(dsps)
    global CDECL(channels)
    global CDECL(commands_sequences)
    global CDECL(commands_channels)

    extern CDECL(dsp_input)
    extern CDECL(output)
    %endif

    %include "core/common.asm"
    %include "core/song/data.asm"
