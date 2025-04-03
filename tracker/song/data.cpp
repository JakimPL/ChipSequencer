#include "data.hpp"

uint16_t bpm = 300;
_Float32 normalizer = 0.25f;

uint8_t num_channels = 0;
uint8_t num_dsps = 0;

uint64_t reference_frequency = 0x2FE15A183;
_Float32 note_divisor = 1.0594630943592953;

Envelopes envelopes;
Sequences sequences;
Orders orders;
Oscillators oscillators;
Wavetables wavetables;
DSPs dsps;
Channels channels;
Offsets buffer_offsets;
Offsets current_offsets = nullptr;
Links links;
