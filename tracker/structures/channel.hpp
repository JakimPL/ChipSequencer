#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <cstdint>

struct Channel {
    uint8_t envelope_index;
    uint8_t order_index;
    uint8_t oscillator_index;
    uint32_t pitch;
    void *output;
    uint8_t output_flag;
};

#endif // CHANNEL_HPP
