#ifndef ENVELOPE_HPP
#define ENVELOPE_HPP

#include <cstdint>

struct Envelope {
    uint16_t base_volume;
    uint16_t sustain_level;
    uint16_t bias;
    uint16_t attack;
    uint16_t decay;
    uint16_t hold;
    uint16_t release;
};

#endif // ENVELOPE_HPP
