#ifndef SONG_DATA_HPP
#define SONG_DATA_HPP

#include "../constants.hpp"
#include "../structures.hpp"
#include "link.hpp"

typedef _Float32 t_output;

/* Read-only internals */
extern "C" t_output output;
extern "C" t_output dsp_input;

extern "C" std::array<uint32_t, MAX_CHANNELS> envelope_timer;
extern "C" std::array<uint32_t, MAX_CHANNELS> sequence_timer;
extern "C" std::array<uint32_t, MAX_CHANNELS> oscillator_timer;
extern "C" std::array<uint8_t, MAX_CHANNELS> pitch;

extern "C" std::array<uint8_t, MAX_CHANNELS> current_sequence;
extern "C" std::array<uint8_t, MAX_CHANNELS> sequence_current_note;

extern "C" uint16_t ticks_per_beat;

/* Modifiable song data */
extern uint16_t bpm;
extern uint16_t sample_rate;
extern _Float32 normalizer;

extern uint8_t num_channels;
extern uint8_t num_dsps;

extern uint64_t reference_frequency;
extern _Float32 note_divisor;

extern Envelopes envelopes;
extern Sequences sequences;
extern Orders orders;
extern Oscillators oscillators;
extern Wavetables wavetables;
extern DSPs dsps;
extern Channels channels;
extern Offsets buffer_offsets;

extern Links links;

#endif // SONG_DATA_HPP