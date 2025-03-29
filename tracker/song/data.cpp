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

uint16_t buffer_offsets[] = {0, 256, 256};
Links links;
