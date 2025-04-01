#ifndef SONG_DATA_HPP
#define SONG_DATA_HPP

#include "../constants.hpp"
#include "../structures.hpp"
#include "link.hpp"

#define SONG_LENGTH 186253
typedef _Float32 t_output;

extern "C" t_output output;
extern "C" t_output dsp_input;
extern "C" uint16_t sample_rate;
extern "C" void calculate_ticks_per_beat();

extern uint16_t bpm;
extern _Float32 normalizer;

extern uint8_t num_channels;
extern uint8_t num_dsps;

extern Envelopes envelopes;
extern Sequences sequences;
extern Orders orders;
extern Oscillators oscillators;
extern Wavetables wavetables;
extern DSPs dsps;
extern Channels channels;

extern "C" Offsets buffer_offsets;
extern Links links;

extern Offsets current_offsets;

#endif // SONG_DATA_HPP