#include "data.hpp"

uint16_t bpm = 300;
_Float32 normalizer = 0.25f;

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
