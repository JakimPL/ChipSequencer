#ifndef STRUCTURES_CHANNEL_HPP
#define STRUCTURES_CHANNEL_HPP

#include <cstdint>
#include <vector>

#include "../constants.hpp"

struct Channel {
    uint8_t envelope_index;
    uint8_t order_index;
    uint8_t oscillator_index;
    uint32_t pitch;
    uint8_t output_flag;
    void *output;
    uint8_t splitter[MAX_OUTPUT_CHANNELS];
};

typedef std::vector<Channel *> Channels;

#endif // STRUCTURES_CHANNEL_HPP
