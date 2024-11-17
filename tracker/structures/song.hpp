#ifndef STRUCTURES_SONG_HPP
#define STRUCTURES_SONG_HPP

#include <cstdint>
#include <vector>
#include "envelope.hpp"
#include "sequence.hpp"
#include "order.hpp"
#include "oscillator.hpp"
#include "wavetable.hpp"
#include "dsp.hpp"
#include "channel.hpp"

struct Song {
    uint16_t bpm;
    uint8_t normalizer;
    int output_channels;
    int song_length;
    std::vector<Envelope> envelopes;
    std::vector<Sequence> sequences;
    std::vector<Order> orders;
    std::vector<Oscillator *> oscillators;
    std::vector<Wavetable> wavetables;
    std::vector<DSP *> dsps;
    std::vector<Channel> channels;

    ~Song() {
        for (auto osc : oscillators) delete osc;
        for (auto dsp : dsps) delete dsp;
    }
};

#endif // STRUCTURES_SONG_HPP
