#ifndef SONG_DATA_HPP
#define SONG_DATA_HPP

#include "../constants.hpp"
#include "../structures.hpp"

#define SONG_LENGTH 186253
typedef _Float32 t_output;

extern "C" t_output output;
extern "C" t_output dsp_input;
extern "C" uint16_t sample_rate;
extern "C" void calculate_ticks_per_beat();

extern uint16_t bpm;
extern _Float32 normalizer;

extern Envelope envelope0;
extern Envelope envelope1;
extern Envelope envelope2;
extern Envelope envelope3;
extern Envelope envelope4;
extern Envelopes envelopes;

extern Sequence sequence0;
extern Sequence sequence1;
extern Sequence sequence2;
extern Sequence sequence3;
extern Sequence sequence4;
extern Sequence sequence5;
extern Sequence sequence6;
extern Sequence sequence7;
extern Sequences sequences;

extern Order order0;
extern Order order1;
extern Order order2;
extern Order order3;
extern Order order4;
extern Order order5;
extern Orders orders;

extern OscillatorSquare oscillator0;
extern OscillatorSaw oscillator1;
extern OscillatorSine oscillator2;
extern OscillatorWavetable oscillator3;
extern Oscillators oscillators;

extern Wavetable wavetable0;
extern Wavetable wavetable1;
extern Wavetables wavetables;

extern DSPDelay dsp0;
extern DSPGainer dsp1;
extern DSPFilter dsp2;
extern DSPs dsps;

extern Channel channel0;
extern Channel channel1;
extern Channel channel2;
extern Channel channel3;
extern Channel channel4;
extern Channel channel5;
extern Channel channel6;
extern Channel channel7;
extern Channel channel8;
extern Channels channels;

extern "C" uint16_t buffer_offsets[];

#endif // SONG_DATA_HPP