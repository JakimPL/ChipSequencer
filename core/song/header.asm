    %define CHANNELS 0
    %define DSPS 0
    %define COMMANDS_CHANNELS 0
    %define WAVETABLES 0

    %define OUTPUT_CHANNELS 2
    %define SONG_LENGTH 0
    %define SAMPLE_RATE 44100
    %define MESSAGE ""

; Tuning
    %define TUNING_FREQUENCY 52612736804
    %define TUNING_NOTE_DIVISOR 1.0594630943592953

    %ifndef BIN
    extern CDECL(num_channels)
    extern CDECL(num_dsps)
    extern CDECL(num_commands_channels)
    extern CDECL(unit)
    %endif
