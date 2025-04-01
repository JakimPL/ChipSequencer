    %define CHANNELS 9
    %define DSPS 3
    %define WAVETABLES 2

    %define OUTPUT_CHANNELS 2
    %define SONG_LENGTH 186253

; Wavetables
    %define WAVETABLE_0_SIZE 16
    %define WAVETABLE_1_SIZE 8
    %define WAVETABLE_SIZE 24

; Buffers
    %define DSP_0_BUFFER_SIZE 256
    %define DSP_1_BUFFER_SIZE 0
    %define DSP_2_BUFFER_SIZE 0
    %define DSP_BUFFER_SIZE 256

    SEGMENT_DATA
num_channels:
    db CHANNELS
num_dsps:
    db DSPS
