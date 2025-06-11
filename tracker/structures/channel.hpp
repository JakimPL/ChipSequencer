#ifndef STRUCTURES_CHANNEL_HPP
#define STRUCTURES_CHANNEL_HPP

#include <cstddef>
#include <cstdint>
#include <fstream>
#include <vector>

#include "../constants.hpp"
#include "../sizes.hpp"

struct Channel {
    uint8_t splitter[MAX_OUTPUT_CHANNELS];
    uint32_t pitch;
    uint8_t target;
    uint8_t output_flag;
    uint8_t flag;
    uint8_t envelope_index;
    uint8_t order_index;
    uint8_t oscillator_index;
    uint8_t fraction;
    uint8_t pad[1];

    void serialize(std::ofstream &file) const;
    static Channel *deserialize(std::ifstream &file);
} __attribute__((packed));

typedef std::vector<Channel *> Channels;

constexpr size_t CHANNEL_SPLITTER = offsetof(Channel, splitter);
constexpr size_t CHANNEL_TARGET = offsetof(Channel, target);
constexpr size_t CHANNEL_OUTPUT_FLAG = offsetof(Channel, output_flag);
constexpr size_t CHANNEL_ENVELOPE_INDEX = offsetof(Channel, envelope_index);
constexpr size_t CHANNEL_ORDER_INDEX = offsetof(Channel, order_index);
constexpr size_t CHANNEL_OSCILLATOR_INDEX = offsetof(Channel, oscillator_index);
constexpr size_t CHANNEL_PITCH = offsetof(Channel, pitch);
constexpr size_t CHANNEL_FLAG = offsetof(Channel, flag);
constexpr size_t CHANNEL_FRACTION = offsetof(Channel, fraction);

static_assert(sizeof(Channel) == SIZE_CHANNEL, "Channel must be of 16 bytes.");

#endif // STRUCTURES_CHANNEL_HPP
