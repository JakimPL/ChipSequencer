#ifndef STRUCTURES_ENVELOPE_HPP
#define STRUCTURES_ENVELOPE_HPP

#include <cstddef>
#include <cstdint>
#include <vector>

#include "../sizes.hpp"

struct Envelope {
    uint16_t base_volume;
    uint16_t sustain_level;
    uint16_t bias;
    uint16_t attack;
    uint16_t decay;
    uint16_t hold;
    uint16_t release;
} __attribute__((packed));

typedef std::vector<Envelope *> Envelopes;

constexpr size_t ENVELOPE_BASE_VOLUME = offsetof(Envelope, base_volume);
constexpr size_t ENVELOPE_SUSTAIN_LEVEL = offsetof(Envelope, sustain_level);
constexpr size_t ENVELOPE_BIAS = offsetof(Envelope, bias);
constexpr size_t ENVELOPE_ATTACK = offsetof(Envelope, attack);
constexpr size_t ENVELOPE_DECAY = offsetof(Envelope, decay);
constexpr size_t ENVELOPE_HOLD = offsetof(Envelope, hold);
constexpr size_t ENVELOPE_RELEASE = offsetof(Envelope, release);

static_assert(sizeof(Envelope) == SIZE_ENVELOPE, "Envelope must be of 14 bytes.");

#endif // STRUCTURES_ENVELOPE_HPP
