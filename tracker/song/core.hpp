#pragma once

#include "../constants.hpp"
#include "../structures.hpp"
#include "links/link.hpp"

/* Read-only internals */
extern "C" std::array<_Float32, MAX_OUTPUT_CHANNELS> output;
extern "C" std::array<_Float32, MAX_DSPS> dsp_input;

extern "C" std::array<uint32_t, MAX_CHANNELS> envelope_timer;
extern "C" std::array<uint32_t, MAX_CHANNELS> oscillator_timer;
extern "C" std::array<uint8_t, MAX_CHANNELS> pitch;
extern "C" std::array<uint8_t, MAX_CHANNELS> envelope_mode;

extern "C" std::array<uint8_t, MAX_CHANNELS> current_sequence;
extern "C" std::array<uint8_t, MAX_CHANNELS> sequence_current_note;
extern "C" std::array<uint32_t, MAX_CHANNELS> sequence_timer;
extern "C" std::array<uint8_t, MAX_CHANNELS> sequence_timer_row;

extern "C" std::array<uint8_t, MAX_COMMANDS_CHANNELS> current_commands_sequence;
extern "C" std::array<uint8_t, MAX_COMMANDS_CHANNELS> commands_sequence_current_command;
extern "C" std::array<uint32_t, MAX_COMMANDS_CHANNELS> commands_sequence_timer;
extern "C" std::array<uint8_t, MAX_COMMANDS_CHANNELS> commands_sequence_timer_row;

extern "C" uint32_t ticks_per_beat;
extern "C" uint16_t global_row;

/* Modifiable song data */
extern uint16_t bpm;
extern _Float32 unit;

extern int32_t sample_rate;
extern _Float32 normalizer;

extern uint8_t num_channels;
extern uint8_t num_dsps;
extern uint8_t num_commands_channels;

extern uint64_t reference_frequency;
extern _Float64 note_divisor;

extern Envelopes envelopes;
extern Sequences sequences;
extern CommandsSequences commands_sequences;
extern Orders orders;
extern Oscillators oscillators;
extern Wavetables wavetables;
extern DSPs dsps;
extern Channels channels;
extern CommandsChannels commands_channels;

extern Targets targets;
extern Links links;
extern CommandsLinks commands_links;
