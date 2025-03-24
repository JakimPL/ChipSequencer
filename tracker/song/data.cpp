#include "data.hpp"

uint16_t bpm = 300;
_Float32 normalizer = 0.25f;

Envelope envelope0 = {0x3F00, 0x1F00, 0x4000, 25, 250, 250, 0};
Envelope envelope1 = {0x7FFF, 0x5FFF, 0x0000, 125, 250, 250, 250};
Envelope envelope2 = {0x2FFF, 0x1FFF, 0x0000, 1500, 1500, 1250, 1250};
Envelope envelope3 = {0x0180, 0x0180, 0x0000, 500, 1, 1, 0};
Envelope envelope4 = {0x5FFF, 0x5FFF, 0x0014, 1, 1, 1, 0};

Envelopes envelopes = {
    &envelope0,
    &envelope1,
    &envelope2,
    &envelope3,
    &envelope4
};

Sequence sequence0 = {16, {{60, 3}, {53, 3}, {51, 3}, {50, 3}, {48, 1}, {-1, 1}, {48, 1}, {46, 1}}};
Sequence sequence1 = {6, {{45, 2}, {55, 1}, {53, 13}}};
Sequence sequence2 = {4, {{24, 1}, {-1, 1}}};
Sequence sequence3 = {4, {{17, 1}, {-1, 1}}};
Sequence sequence4 = {4, {{48, 16}, {48, 16}}};
Sequence sequence5 = {4, {{51, 16}, {51, 16}}};
Sequence sequence6 = {4, {{55, 16}, {53, 16}}};
Sequence sequence7 = {4, {{58, 16}, {57, 16}}};

Sequences sequences = {
    &sequence0,
    &sequence1,
    &sequence2,
    &sequence3,
    &sequence4,
    &sequence5,
    &sequence6,
    &sequence7
};

Order order0 = {4, {0, 1, 0, 1}};
Order order1 = {16, {2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3}};
Order order2 = {1, {4}};
Order order3 = {1, {5}};
Order order4 = {1, {6}};
Order order5 = {1, {7}};

Orders orders = {
    &order0,
    &order1,
    &order2,
    &order3,
    &order4,
    &order5
};

OscillatorSquare oscillator0 = {2, OSCILLATOR_SQUARE, 0x20};
OscillatorSaw oscillator1;
OscillatorSine oscillator2;
OscillatorWavetable oscillator3 = {2, OSCILLATOR_WAVETABLE, 1};

Oscillators oscillators = {
    &oscillator0,
    &oscillator1,
    &oscillator2,
    &oscillator3
};

Wavetable wavetable0 = {16, {0x00, 0x1F, 0x3F, 0x5F, 0x7F, 0x9F, 0xBF, 0xDF, 0xFF, 0xDF, 0xBF, 0x9F, 0x7F, 0x5F, 0x3F, 0x1F}};
Wavetable wavetable1 = {8, {0x00, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70}};

Wavetables wavetables = {
    &wavetable0,
    &wavetable1
};

DSPDelay dsp0 = {DSP_DELAY_SIZE, EFFECT_DELAY, &output, 0, 0x7FFF, 0x6FFF, 0x5FFF, 256};
DSPGainer dsp1 = {DSP_GAINER_SIZE, EFFECT_GAINER, &output, 0, 0x9FFF};
DSPFilter dsp2 = {DSP_FILTER_SIZE, EFFECT_FILTER, &output, 0, 336};

DSPs dsps = {
    &dsp0,
    &dsp1,
    &dsp2
};

Channel channel0 = {1, 0, 3, 0x02000000, &output, 0};
Channel channel1 = {1, 1, 1, 0x02000000, &output, 0};
Channel channel2 = {2, 2, 0, 0x02000000, &output, 0};
Channel channel3 = {2, 3, 0, 0x02000000, &output, 0};
Channel channel4 = {2, 4, 0, 0x02000000, &output, 0};
Channel channel5 = {2, 5, 0, 0x02000000, &output, 0};
Channel channel6 = {4, 0xFF, 1, 0x5000, &(dsp2.frequency), 0b01010111};
Channel channel7 = {3, 0xFF, 2, 0x8000, &(channel0.pitch), 0b01110110};
Channel channel8 = {0, 0xFF, 2, 0x14800, &(oscillator0.duty_cycle), 0b01101000};

Channels channels = {
    &channel0,
    &channel1,
    &channel2,
    &channel3,
    &channel4,
    &channel5,
    &channel6,
    &channel7,
    &channel8
};

uint16_t buffer_offsets[] = {0, 256, 256};
