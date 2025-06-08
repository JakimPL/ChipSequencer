#include "core.hpp"

uint16_t bpm = DEFAULT_BPM;
_Float32 unit = DEFAULT_UNIT;
int32_t sample_rate = DEFAULT_SAMPLE_RATE;
_Float32 normalizer = DEFAULT_NORMALIZER;

uint8_t num_channels = 0;
uint8_t num_dsps = 0;
uint8_t num_commands_channels = 0;

uint64_t reference_frequency = 0x2FE15A183;
_Float64 note_divisor = DEFAULT_NOTE_DIVISOR;

Envelopes envelopes;
Sequences sequences;
CommandsSequences commands_sequences;
Orders orders;
Oscillators oscillators;
Wavetables wavetables;
DSPs dsps;
Channels channels;
CommandsChannels commands_channels;

Targets targets;
Links links;
CommandsLinks commands_links;
