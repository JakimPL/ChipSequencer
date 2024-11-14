#include "constants.hpp"
#include "structures.hpp"

extern "C" _Float32 output;

uint16_t bpm = 600;
uint16_t normalizer = 6;

Envelope envelopes[] = {
    {0x3F00, 0x1F00, 0x4000, 25, 250, 250, 0},
    {0x7FFF, 0x5FFF, 0x0000, 125, 250, 250, 250},
    {0x2FFF, 0x1FFF, 0x0000, 1500, 1500, 1250, 1250},
    {0x0180, 0x0180, 0x0000, 500, 1, 1, 0},
    {0x5FFF, 0x5FFF, 0x0014, 1, 1, 1, 0}};

Sequence sequences = {16, {{60, 3}, {53, 3}, {51, 3}, {50, 3}, {48, 1}, {-1, 1}, {48, 1}, {46, 1}}};
Sequence sequence2 = {6, {{45, 2}, {55, 1}, {53, 13}}};
Sequence sequence3 = {6, {{45, 2}, {55, 1}, {53, 13}}};
Sequence sequence4 = {4, {{24, 1}, {-1, 1}}};
Sequence sequence5 = {4, {{17, 1}, {-1, 1}}};
Sequence sequence6 = {4, {{48, 16}, {48, 16}}};
Sequence sequence7 = {4, {{51, 16}, {51, 16}}};
Sequence sequence8 = {4, {{55, 16}, {53, 16}}};
Sequence sequence9 = {4, {{58, 16}, {57, 16}}};

Order orders = {4, {0, 1, 0, 1}};
Order order2 = {16, {2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2}};
Order order3 = {1, {4}};
Order order5 = {1, {5}};
Order order6 = {1, {6}};
Order order7 = {1, {7}};

OscillatorSquare oscillators = {2, OSCILLATOR_SQUARE, 0x20};
OscillatorSaw oscillator2;
OscillatorSine oscillator3;
OscillatorWavetable oscillator4 = {2, OSCILLATOR_WAVETABLE, 1};

Wavetable wavetables = {16, {0x00, 0x1F, 0x3F, 0x5F, 0x7F, 0x9F, 0xBF, 0xDF, 0xFF, 0xDF, 0xBF, 0x9F, 0x7F, 0x5F, 0x3F, 0x1F}};
Wavetable wavetable2 = {8, {0x00, 0x20, 0xE0, 0x40, 0xC0, 0x60, 0xA0, 0x80}};

DSPDelay dsps = {15, EFFECT_DELAY, &output, 0, 0x7FFF, 0x6FFF, 0x5FFF, 256};
DSPGainer dsp2 = {9, EFFECT_GAINER, &output, 0, 0x9FFF};
DSPFilter dsp3 = {9, EFFECT_FILTER, &output, 0, 336};

Channel channels[] = {
    {4, 0xFF, 1, 0x5000, &output, 0b01010111},
    {3, 0xFF, 2, 0x80000, &output, 0b01110110},
    {0, 0xFF, 2, 0x14800, &output, 0b01101000},
    {1, 0, 3, 0x02000000, &output, 0},
    {1, 1, 1, 0x02000000, &output, 0},
    {2, 2, 0, 0x02000000, &output, 0},
    {2, 3, 0, 0x02000000, &output, 0},
    {2, 4, 0, 0x02000000, &output, 0},
    {2, 5, 0, 0x02000000, &output, 0},
};

uint16_t buffer_offsets[] = {0, 256, 256};
uint16_t wavetable_offsets[] = {0, 16};