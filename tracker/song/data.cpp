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

Sequences sequences = {};
Orders orders = {};

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

Wavetables wavetables = {};

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
Channel channel6 = {4, 0xFF, 1, 0x5000, &dsp2.frequency, 0b01010111};
Channel channel7 = {3, 0xFF, 2, 0x8000, &channel7.pitch, 0b01110110};
Channel channel8 = {0, 0xFF, 2, 0x44800, &oscillator0.duty_cycle, 0b01101000};

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

Links links = {
    {
        {ItemType::CHANNEL, 0, Target::OUTPUT_CHANNEL, 0, 0},
        {ItemType::CHANNEL, 1, Target::OUTPUT_CHANNEL, 0, 0},
        {ItemType::CHANNEL, 2, Target::OUTPUT_CHANNEL, 0, 0},
        {ItemType::CHANNEL, 3, Target::OUTPUT_CHANNEL, 0, 0},
        {ItemType::CHANNEL, 4, Target::OUTPUT_CHANNEL, 0, 0},
        {ItemType::CHANNEL, 5, Target::OUTPUT_CHANNEL, 0, 0},
        {ItemType::CHANNEL, 6, Target::OUTPUT_CHANNEL, 0, 0},
        {ItemType::CHANNEL, 7, Target::OUTPUT_CHANNEL, 0, 0},
        {ItemType::CHANNEL, 8, Target::OUTPUT_CHANNEL, 0, 0},
        // {ItemType::CHANNEL, 6, Target::DSP, 2, DSP_FILTER_FREQUENCY},
        // {ItemType::CHANNEL, 7, Target::CHANNEL, 7, CHANNEL_PITCH},
        // {ItemType::CHANNEL, 8, Target::OSCILLATOR, 0, OSCILLATOR_SQUARE_DUTY_CYCLE},
    },
    {
        {ItemType::DSP, 0, Target::OUTPUT_CHANNEL, 0, 0},
        {ItemType::DSP, 1, Target::OUTPUT_CHANNEL, 0, 0},
        {ItemType::DSP, 2, Target::OUTPUT_CHANNEL, 0, 0},
    }
};
