#include "data.hpp"

uint16_t bpm = 300;
_Float32 normalizer = 0.25f;

Envelopes envelopes = {};
Sequences sequences = {};
Orders orders = {};
Oscillators oscillators = {};
Wavetables wavetables = {};
DSPs dsps = {};
Channels channels = {};
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
