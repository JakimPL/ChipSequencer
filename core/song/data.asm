    SEGMENT_DATA
bpm:
    dw 300
unit:
    dd 60.0
normalizer:
    dd 0.5

num_channels:
    db CHANNELS
num_dsps:
    db DSPS
num_commands_channels:
    db COMMANDS_CHANNELS

envelopes:
sequences:
commands_sequences:
orders:
oscillators:
wavetables:
dsps:
channels:
commands_channels:

    align 4
targets:
    dd output

buffer_offsets:
