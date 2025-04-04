#ifndef STRUCTURES_ENVELOPE_HPP
#define STRUCTURES_ENVELOPE_HPP

#include <cstdint>
#include <vector>

struct Envelope {
    uint16_t base_volume;
    uint16_t sustain_level;
    uint16_t bias;
    uint16_t attack;
    uint16_t decay;
    uint16_t hold;
    uint16_t release;
};

typedef std::vector<Envelope *> Envelopes;

#endif // STRUCTURES_ENVELOPE_HPP
